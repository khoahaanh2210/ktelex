"""Exercise the built engine through a private IBus daemon (no desktop edits)."""
import os
from pathlib import Path
import subprocess
import sys
import tempfile
import time
from xml.sax.saxutils import escape


def require(condition, message):
    if not condition:
        raise AssertionError(message)


def main():
    executable = Path(sys.argv[1]).resolve()
    daemon_path = sys.argv[2]
    # All discovery files must be isolated BEFORE initializing libibus.
    with tempfile.TemporaryDirectory(prefix="ktlex-dbus-test-") as temporary:
        root = Path(temporary)
        components = root / "components"
        components.mkdir()
        (components / "ktlex.xml").write_text(
            '<component><name>org.freedesktop.IBus.Ktlex</name>'
            '<description>ktlex regression test</description><exec>'
            + escape(str(executable)) + '</exec><version>0.1.0</version>'
            '<engines><engine><name>ktlex</name><longname>ktlex test</longname>'
            '<language>vi</language><layout>us</layout></engine></engines></component>'
        )
        os.environ.update({
            "XDG_CONFIG_HOME": str(root / "config"),
            "XDG_CACHE_HOME": str(root / "cache"),
            "IBUS_ADDRESS_FILE": str(root / "address"),
            "IBUS_ADDRESS": "unix:path=" + str(root / "bus.sock"),
            "IBUS_COMPONENT_PATH": str(components),
            "GIO_USE_VFS": "local",
        })
        import gi
        gi.require_version("IBus", "1.0")
        from gi.repository import IBus, GLib
        IBus.init()
        require(IBus.get_socket_path() == str(root / "address"), "IBus discovery must stay isolated")
        with (root / "daemon.log").open("w+") as log:
            daemon = subprocess.Popen([
                daemon_path, "--address", os.environ["IBUS_ADDRESS"], "--cache=none",
                "--panel=disable", "--config=disable", "--emoji-extension=disable",
            ], stdout=log, stderr=log)
            bus = None
            try:
                deadline = time.monotonic() + 5
                while not (root / "bus.sock").exists() and time.monotonic() < deadline:
                    require(daemon.poll() is None, "private IBus daemon stopped")
                    time.sleep(.01)
                bus = IBus.Bus.new()
                require(bus.is_connected(), "private IBus bus must connect")
                context = bus.create_input_context("ktlex-regression-test")
                state = {"document": "", "preedit": "", "updates": 0, "commits": 0, "deletes": 0, "mode": None}
                def commit(_context, text):
                    state["document"] += text.get_text()
                    state["commits"] += 1
                def preedit(_context, text, cursor, visible, mode):
                    state["preedit"] = text.get_text() if visible else ""
                    if mode is not None:
                        state["mode"] = mode
                    state["updates"] += 1
                def delete(*_arguments):
                    # Deliberately reject deletion, as affected applications do.
                    state["deletes"] += 1
                context.connect("commit-text", commit)
                context.connect("update-preedit-text-with-mode", preedit)
                context.connect("update-preedit-text", lambda c, t, p, v: preedit(c, t, p, v, None))
                context.connect("delete-surrounding-text", delete)
                context.set_capabilities(IBus.Capabilite.FOCUS | IBus.Capabilite.PREEDIT_TEXT | IBus.Capabilite.SURROUNDING_TEXT)
                context.focus_in()
                context.set_engine("ktlex")
                loop = GLib.MainContext.default()
                def wait(predicate, message):
                    end = time.monotonic() + 5
                    while time.monotonic() < end:
                        while loop.pending():
                            loop.iteration(False)
                        if predicate():
                            return
                        time.sleep(.002)
                    require(False, message + ": " + repr(state))
                def press(key):
                    before = state["updates"]
                    require(context.process_key_event(key, 0, 0), "key must be handled")
                    wait(lambda: state["updates"] > before, "preedit update missing")
                def engine_ready():
                    engine = context.get_engine()
                    return engine is not None and engine.get_name() == "ktlex"
                wait(engine_ready, "built engine did not start")
                context.set_client_commit_preedit(False)
                words = [
                    ("haf", "hà"), ("has", "há"), ("har", "hả"), ("hax", "hã"), ("haj", "hạ"),
                    ("dienej", "diện"), ("dienje", "diện"), ("tienges", "tiếng"),
                    ("Quas", "Quá"), ("GIAS", "GIÁ"), ("dooom", "doom"),
                    ("ddawng", "đăng"), ("thuowngf", "thường"), ("zoom", "zoom"),
                    ("giayas", "giấy"), ("quayas", "quấy"),
                    ("ddaya", "đây"), ("DDAYA", "ĐÂY"), ("ddaysa", "đấy"),
                    ("cauaf", "cầu"), ("toio", "tôi"), ("tieues", "tiếu"), ("yeue", "yêu"),
                    ("tuoiws", "tưới"), ("guiwr", "gửi"), ("cuuws", "cứu"), ("huouw", "hươu"),
                    ("nhapaj", "nhập"), ("nhapja", "nhập"), ("NHAPAJ", "NHẬP"),
                    ("ddapaj", "đập"), ("bepes", "bếp"), ("lenhej", "lệnh"),
                    ("hopoj", "hộp"), ("luataj", "luật"), ("hueches", "huếch"), ("nguechej", "nguệch"), ("data", "data"), ("coco", "coco"),
                    ("quawms", "quắm"), ("Quowr", "Quở"), ("uww", "uw"),
                ]
                for keys, expected in words:
                    state["document"] = ""
                    state["commits"] = 0
                    # Never supply surrounding text despite advertising support.
                    # In rapid mode, do not dispatch client updates between keys.
                    for key in keys:
                        require(context.process_key_event(ord(key), 0, 0), "letter must be handled: " + keys + " / " + key)
                    wait(lambda: state["preedit"] == expected, "incorrect composed word " + keys)
                    require(state["document"] == "" and state["commits"] == 0, "letters must not be committed individually")
                    press(IBus.KEY_space)
                    wait(lambda: state["document"] == expected + " ", "wrong committed word " + keys)
                    require(state["commits"] == 1 and state["preedit"] == "", "word must commit once")
                # Daemon-owned focus commit (GTK-style client).
                state.update(document="", commits=0)
                for key in "haf":
                    press(ord(key))
                before = state["updates"]
                context.focus_out()
                wait(lambda: state["document"] == "hà" and state["updates"] > before, "focus out lost word")
                require(state["commits"] == 1, "focus out duplicated word")
                context.focus_in()
                # GNOME/Mutter owns this preedit commit before requesting Reset.
                context.set_client_commit_preedit(True)
                state.update(document="", commits=0)
                for key in "haf":
                    press(ord(key))
                require(state["mode"] == IBus.PreeditFocusMode.COMMIT, "focus-out requires COMMIT mode")
                state["document"] = state["preedit"]
                state["commits"] = 1
                before = state["updates"]
                context.reset()
                wait(lambda: state["updates"] > before and state["preedit"] == "", "client-owned reset did not clear preedit")
                before = state["updates"]
                context.focus_out()
                wait(lambda: state["updates"] > before, "focus-out callback missing")
                require(state["document"] == "hà" and state["commits"] == 1, "GNOME reset/focus-out duplicated text")
                require(state["deletes"] == 0, "engine must not rely on surrounding deletion")
                print(f"{len(words)} rapid-input cases and both focus-commit owners passed through real IBus D-Bus")
            except Exception:
                log.seek(0)
                print(log.read(), file=sys.stderr)
                raise
            finally:
                if bus is not None and bus.is_connected():
                    bus.exit(False)
                daemon.terminate()
                try:
                    daemon.wait(timeout=3)
                except subprocess.TimeoutExpired:
                    daemon.kill()
                    daemon.wait()


if __name__ == "__main__":
    main()
