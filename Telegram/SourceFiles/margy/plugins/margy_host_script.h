#pragma once

namespace Margy::Plugins {

inline constexpr auto kMargyHostScript = R"py(
# -*- coding: utf-8 -*-
"""MargyDesktop plugin host runtime.

Runs Python plugins (.marp) with full Android Margelet compatibility.
Communicates with MargyDesktop via JSON lines over stdin/stdout.
"""

import importlib.util
import json
import math
import os
import sys
import threading
import time
import traceback
import types

sys.stdout.reconfigure(encoding='utf-8')
sys.stdin.reconfigure(encoding='utf-8')

_real_stdout = sys.stdout
_real_stderr = sys.stderr

_plugins = {}
_loaded = {}
_settings_specs = {}
_hooks_enabled = True
_active_fields = {}
_current_activity = None


def send_ipc(obj):
    try:
        line = json.dumps(obj, ensure_ascii=False)
        _real_stdout.write(line + "\n")
        _real_stdout.flush()
    except Exception as e:
        _real_stderr.write("IPC write error: " + str(e) + "\n")


class _Console:
    def __init__(self, name, is_error=False):
        self._name = name
        self._error = is_error
        self._buffer = ""

    def write(self, text):
        self._buffer += text
        while "\n" in self._buffer:
            line, self._buffer = self._buffer.split("\n", 1)
            if line:
                send_ipc({"op": "log", "plugin": self._name, "text": line, "error": self._error})

    def flush(self):
        if self._buffer:
            send_ipc({"op": "log", "plugin": self._name, "text": self._buffer, "error": self._error})
            self._buffer = ""


# --- Mock Java & Android environment ---

class JavaSpanned:
    SPAN_EXCLUSIVE_EXCLUSIVE = 33


class JavaForegroundColorSpan:
    def __init__(self, color):
        self.color = color


class JavaGradientDrawable:
    OVAL = 1
    RECTANGLE = 0

    def __init__(self):
        self.shape = 0
        self.color = 0
        self.alpha = 255
        self.bounds = (0, 0, 0, 0)

    def setShape(self, shape):
        self.shape = shape

    def setColor(self, color):
        self.color = color

    def setAlpha(self, alpha):
        self.alpha = alpha

    def setBounds(self, l, t, r, b):
        self.bounds = (float(l), float(t), float(r), float(b))

    def getIntrinsicWidth(self):
        return 4

    def getPaint(self):
        return JavaPaint()


class JavaPaint:
    def __init__(self, char_width=10.0, line_height=20.0):
        self._char_width = char_width
        self._line_height = line_height
        self._color = -1
        self._anti_alias = True

    def measureText(self, text):
        return float(len(text) * self._char_width)

    def getColor(self):
        return self._color

    def setAntiAlias(self, val):
        self._anti_alias = bool(val)

    def setARGB(self, a, r, g, b):
        c = ((a & 0xFF) << 24) | ((r & 0xFF) << 16) | ((g & 0xFF) << 8) | (b & 0xFF)
        self._color = c - (1 << 32) if c >= (1 << 31) else c


class JavaLayout:
    def __init__(self, text, char_width=10.0, line_height=20.0):
        self.text = text or ""
        self.char_width = char_width
        self.line_height = line_height
        self.lines = self.text.split("\n") if self.text else [""]

    def getLineCount(self):
        return len(self.lines)

    def getLineStart(self, idx):
        if idx >= len(self.lines):
            return len(self.text)
        s = 0
        for i in range(idx):
            s += len(self.lines[i]) + 1
        return s

    def getLineEnd(self, idx):
        if idx >= len(self.lines):
            return len(self.text)
        return self.getLineStart(idx) + len(self.lines[idx])

    def getLineLeft(self, idx):
        return 0.0

    def getLineTop(self, idx):
        return float(idx * self.line_height)


class JavaSpannableString:
    def __init__(self, text):
        self.text = text or ""
        self.spans = {}

    def toString(self):
        return self.text

    def length(self):
        return len(self.text)

    def setSpan(self, span, start, end, flags):
        self.spans[id(span)] = (span, start, end, flags)

    def removeSpan(self, span):
        self.spans.pop(id(span), None)


class JavaOverlay:
    def __init__(self):
        self.drawables = []

    def add(self, drawable):
        if drawable not in self.drawables:
            self.drawables.append(drawable)

    def remove(self, drawable):
        if drawable in self.drawables:
            self.drawables.remove(drawable)


class JavaEditTextBoldCursor:
    def __init__(self, field_id="default"):
        self.field_id = field_id
        self.text_content = ""
        self.spannable = JavaSpannableString("")
        self.overlay = JavaOverlay()
        self.cursor_visible = True
        self.char_width = 9.0
        self.line_height = 20.0
        self.padding_left = 6
        self.padding_top = 4
        self.needs_anim = False
        self.cursor_pos = 0

    def update_state(self, text, cursor_pos, char_w, line_h, pad_l, pad_t):
        self.text_content = text or ""
        self.spannable = JavaSpannableString(self.text_content)
        self.cursor_pos = cursor_pos
        if char_w > 0:
            self.char_width = float(char_w)
        if line_h > 0:
            self.line_height = float(line_h)
        if pad_l >= 0:
            self.padding_left = int(pad_l)
        if pad_t >= 0:
            self.padding_top = int(pad_t)

    def getText(self):
        return self.spannable

    def getLayout(self):
        return JavaLayout(self.text_content, self.char_width, self.line_height)

    def getPaint(self):
        return JavaPaint(self.char_width, self.line_height)

    def getCurrentTextColor(self):
        return -1

    def getTotalPaddingLeft(self):
        return self.padding_left

    def getTotalPaddingTop(self):
        return self.padding_top

    def getOverlay(self):
        return self.overlay

    def getTextCursorDrawable(self):
        return JavaGradientDrawable()

    def isCursorVisible(self):
        return self.cursor_visible

    def setCursorVisible(self, visible):
        self.cursor_visible = bool(visible)

    def hashCode(self):
        return abs(hash(self.field_id)) & 0x7FFFFFFF

    def postInvalidateOnAnimation(self):
        self.needs_anim = True


class JavaBitmapConfig:
    ARGB_8888 = 1


class JavaBitmap:
    Config = JavaBitmapConfig

    def __init__(self, w, h):
        self.width = w
        self.height = h
        self.particles = []

    @staticmethod
    def createBitmap(w, h, config=None):
        return JavaBitmap(w, h)


class JavaCanvas:
    def __init__(self, bitmap):
        self.bitmap = bitmap

    def drawCircle(self, x, y, r, paint=None):
        if self.bitmap is not None:
            self.bitmap.particles.append({"x": float(x), "y": float(y), "r": float(r)})


class JavaImageView:
    def __init__(self, context=None):
        self._context = context
        self._parent = None

    def setClickable(self, val): pass
    def setFocusable(self, val): pass
    def setFocusableInTouchMode(self, val): pass
    def setBackgroundColor(self, color): pass

    def setImageBitmap(self, bitmap):
        if bitmap is not None:
            send_ipc({"op": "snow_frame", "particles": bitmap.particles})

    def getParent(self):
        return self._parent


class JavaLayoutParams:
    MATCH_PARENT = -1
    WRAP_CONTENT = -2

    def __init__(self, w, h):
        self.width = w
        self.height = h


class JavaViewGroup:
    def __init__(self, w=1200, h=800):
        self._width = w
        self._height = h
        self._children = []

    def getWidth(self):
        return self._width

    def getHeight(self):
        return self._height

    def addView(self, view, params=None):
        if view not in self._children:
            self._children.append(view)
            view._parent = self

    def removeView(self, view):
        if view in self._children:
            self._children.remove(view)
            view._parent = None
            send_ipc({"op": "snow_frame", "particles": []})


class JavaActivity:
    def __init__(self, chat_id=0, w=1200, h=800):
        self.chat_id = chat_id
        self._root = JavaViewGroup(w, h)

    def getFragmentView(self):
        return self._root

    def getContentView(self):
        return self._root

    def getContext(self):
        return self


class JavaLooper:
    @staticmethod
    def getMainLooper():
        return object()


class JavaHandler:
    def __init__(self, looper=None):
        self._lock = threading.Lock()
        self._tasks = {}

    def post(self, runnable):
        self.postDelayed(runnable, 0)

    def postDelayed(self, runnable, delay_ms):
        def fire():
            with self._lock:
                self._tasks.pop(id(runnable), None)
            if hasattr(runnable, "run"):
                runnable.run()
            elif callable(runnable):
                runnable()

        with self._lock:
            old_t = self._tasks.pop(id(runnable), None)
            if old_t:
                old_t.cancel()
            t = threading.Timer(max(0.001, delay_ms / 1000.0), fire)
            t.daemon = True
            self._tasks[id(runnable)] = t
            t.start()

    def removeCallbacks(self, runnable):
        with self._lock:
            t = self._tasks.pop(id(runnable), None)
            if t:
                t.cancel()


class MethodHookParam:
    def __init__(self, this_obj=None, args=None):
        self.thisObject = this_obj
        self.args = list(args or [])
        self._result = None
        self._cancelled = False

    def getResult(self):
        return self._result

    def setResult(self, res):
        self._result = res
        self._cancelled = True


class RegisteredHook:
    def __init__(self, target_class, method_name, before=None, after=None):
        self.target_class = target_class
        self.method_name = method_name
        self.before = before
        self.after = after


_active_method_hooks = []


class JavaModule:
    @staticmethod
    def jclass(name):
        if name == "android.text.Spanned":
            return JavaSpanned
        if name == "android.text.style.ForegroundColorSpan":
            return JavaForegroundColorSpan
        if name == "android.graphics.drawable.GradientDrawable":
            return JavaGradientDrawable
        if name == "org.telegram.ui.Components.EditTextBoldCursor":
            return JavaEditTextBoldCursor
        if name == "android.view.ViewGroup":
            return JavaViewGroup
        if name == "android.widget.ImageView":
            return JavaImageView
        if name == "android.graphics.Bitmap":
            return JavaBitmap
        if name == "android.graphics.Canvas":
            return JavaCanvas
        if name == "android.graphics.Paint":
            return JavaPaint
        if name in ("android.view.ViewGroup$LayoutParams", "android.view.ViewGroup.LayoutParams"):
            return JavaLayoutParams
        if name == "android.os.Handler":
            return JavaHandler
        if name == "android.os.Looper":
            return JavaLooper
        if name == "java.lang.Runnable":
            return object
        if name == "org.telegram.margelet.MargeletPluginHost":
            return HostProxy
        if name == "org.telegram.margelet.MargeletHooks":
            return HooksProxy
        if name == "org.telegram.margelet.MargeletHookEngine":
            return HookEngineProxy
        if name == "org.telegram.margelet.MargeletFiles":
            return FilesProxy
        if name == "org.telegram.messenger.AndroidUtilities":
            return AndroidUtilitiesProxy
        return type(name.split(".")[-1], (), {})

    @staticmethod
    def dynamic_proxy(cls):
        return object

    @staticmethod
    def jarray(elem_type):
        return lambda items: list(items)


class HostProxy:
    @staticmethod
    def log(plugin, text, is_error=False):
        send_ipc({"op": "log", "plugin": str(plugin), "text": str(text), "error": bool(is_error)})

    @staticmethod
    def toast(text):
        send_ipc({"op": "toast", "text": str(text)})

    @staticmethod
    def get(plugin_id, key, fallback=None):
        return _plugins.get(plugin_id, {})._prefs.get(str(key), fallback)

    @staticmethod
    def set(plugin_id, key, value):
        if plugin_id in _plugins:
            if value is None:
                _plugins[plugin_id]._prefs.pop(str(key), None)
            else:
                _plugins[plugin_id]._prefs[str(key)] = str(value)
        send_ipc({"op": "set_pref", "plugin": str(plugin_id), "key": str(key), "value": value})


class HooksProxy:
    CANCEL = "\u0000margelet-cancel"

    @staticmethod
    def declare(plugin_id, settings_json):
        try:
            parsed = json.loads(settings_json)
            send_ipc({"op": "declare_settings", "plugin": plugin_id, "settings": parsed})
        except Exception:
            pass


class HookEngineProxy:
    @staticmethod
    def working():
        return _hooks_enabled

    @staticmethod
    def enabled():
        return _hooks_enabled

    @staticmethod
    def failure():
        return ""


class FilesProxy:
    @staticmethod
    def pick(plugin_id, mime_type, callback):
        pass


class AndroidUtilitiesProxy:
    @staticmethod
    def runOnUIThread(runnable, delay_ms=0):
        if hasattr(runnable, "run"):
            runnable.run()
        elif callable(runnable):
            runnable()


# Install fake java modules into sys.modules
sys.modules["java"] = JavaModule
sys.modules["java.lang"] = type("lang", (), {"Runnable": object})


# --- Margelet plugin object ---

class Margelet:
    def __init__(self, plugin_id, name, folder):
        self.id = plugin_id
        self.name = name
        self.folder = folder
        self._prefs = {}
        self._on_chat_opened = []
        self._on_send = []
        self._on_send_photo = []
        self._on_message = []
        self._on_settings = []
        self._on_deleted = []
        self._on_pin = []
        self._on_request = []
        self._on_answer = []
        self._on_update = []
        self._buttons = {}
        self._actions = {}
        self._cancel_send = False

    def log(self, *parts):
        text = " ".join(str(p) for p in parts)
        send_ipc({"op": "log", "plugin": self.name, "text": text, "error": False})

    def error(self, *parts):
        text = " ".join(str(p) for p in parts)
        send_ipc({"op": "log", "plugin": self.name, "text": text, "error": True})

    def ui(self, call, delay_ms=0):
        call()
        return None

    def every(self, ms, call):
        return None

    def cancel(self, task=None):
        if task is None:
            self._cancel_send = True
            return False
        return None

    def toast(self, text):
        send_ipc({"op": "toast", "text": str(text)})

    def get(self, key, fallback=None):
        val = self._prefs.get(str(key))
        return fallback if val is None else val

    def set(self, key, value):
        if value is None:
            self._prefs.pop(str(key), None)
        else:
            self._prefs[str(key)] = str(value)
        send_ipc({"op": "set_pref", "plugin": self.id, "key": str(key), "value": value})

    def flag(self, key, fallback=False):
        val = self.get(key, None)
        if val is None:
            return bool(fallback)
        if isinstance(val, bool):
            return val
        return str(val).lower() in ("1", "true", "yes", "on")

    def background(self, call):
        try:
            call()
        except Exception:
            self.error(traceback.format_exc())

    def send(self, chat, text):
        send_ipc({"op": "send_msg", "chat": chat, "text": text})

    def fetch(self, url, call):
        def worker():
            res = None
            try:
                import urllib.request
                req = urllib.request.Request(url, headers={"User-Agent": "MargyDesktop"})
                with urllib.request.urlopen(req, timeout=10) as r:
                    res = r.read().decode("utf-8", errors="replace")
            except Exception:
                pass
            try:
                call(res)
            except Exception:
                self.error(traceback.format_exc())
        threading.Thread(target=worker, daemon=True).start()

    def activity(self):
        return _current_activity

    def window(self, title, view):
        pass

    def color(self, value):
        val = int(value)
        return val - (1 << 32) if val >= (1 << 31) else val

    # Settings API
    def header(self, text):
        return {"kind": "header", "title": str(text)}

    def note(self, text):
        return {"kind": "note", "title": str(text)}

    def switch(self, key, title, default=False, about=None):
        return {"kind": "switch", "key": str(key), "title": str(title), "default": bool(default), "about": about}

    def text(self, key, title, default="", about=None):
        return {"kind": "text", "key": str(key), "title": str(title), "default": str(default), "about": about}

    def choice(self, key, title, options, default=None):
        opts = list(options)
        return {"kind": "choice", "key": str(key), "title": str(title), "options": opts,
                "default": opts[0] if default is None and opts else default}

    def action(self, title, call, danger=False, key=None):
        action_key = str(key or title)
        self._actions[action_key] = call
        return {"kind": "action", "key": action_key, "title": str(title), "danger": bool(danger)}

    def settings(self, *rows):
        spec = []
        for r in rows:
            if not r:
                continue
            spec.append(r)
            if "key" in r and "default" in r:
                k = r["key"]
                if self.get(k) is None:
                    self.set(k, r["default"])
        _settings_specs[self.id] = spec
        send_ipc({"op": "declare_settings", "plugin": self.id, "settings": spec})

    def on_settings(self, call):
        self._on_settings.append(call)

    # Events API
    def on_chat_opened(self, call):
        self._on_chat_opened.append(call)

    def on_send(self, call):
        self._on_send.append(call)

    def on_send_photo(self, call):
        self._on_send_photo.append(call)

    def on_message(self, call):
        self._on_message.append(call)

    def on_deleted(self, call):
        self._on_deleted.append(call)

    def on_pin(self, call):
        self._on_pin.append(call)

    def on_request(self, call):
        self._on_request.append(call)

    def on_answer(self, call):
        self._on_answer.append(call)

    def on_update(self, call):
        self._on_update.append(call)

    def button(self, title, call):
        self._buttons["chat\u0000" + str(title)] = (call, "chat")
        send_ipc({"op": "add_button", "plugin": self.id, "title": str(title), "where": "chat"})

    def menu(self, where, title, call):
        self._buttons[str(where) + "\u0000" + str(title)] = (call, str(where))
        send_ipc({"op": "add_button", "plugin": self.id, "title": str(title), "where": str(where)})

    def pick_file(self, call, types=None):
        call(None)

    # Method hooks API
    def hooks_work(self):
        return _hooks_enabled

    def hooks_why(self):
        return "" if _hooks_enabled else "hooks disabled"

    def hook(self, where, method, before=None, after=None, args=None):
        if not _hooks_enabled:
            return False
        _active_method_hooks.append(RegisteredHook(str(where), str(method), before, after))
        return True


def run_plugin(plugin_id, name, folder, prefs=None):
    if plugin_id in _loaded:
        return
    main_py = os.path.join(folder, "main.py")
    if not os.path.exists(main_py):
        send_ipc({"op": "log", "plugin": name, "text": "main.py not found in " + folder, "error": True})
        return

    m = Margelet(plugin_id, name, folder)
    if prefs:
        m._prefs.update(prefs)
    _plugins[plugin_id] = m

    out, err = sys.stdout, sys.stderr
    sys.stdout = _Console(name, False)
    sys.stderr = _Console(name, True)
    try:
        if folder not in sys.path:
            sys.path.insert(0, folder)
        spec = importlib.util.spec_from_file_location("margelet_plugin_" + plugin_id, main_py)
        module = importlib.util.module_from_spec(spec)
        module.margelet = m
        spec.loader.exec_module(module)
        _loaded[plugin_id] = module
        if hasattr(module, "on_start"):
            module.on_start()
        m.log("плагин запущен")
    except Exception as e:
        frames = e.__traceback__
        if frames and frames.tb_next:
            frames = frames.tb_next
        tb = "".join(traceback.format_exception(type(e), e, frames))
        send_ipc({"op": "log", "plugin": name, "text": tb, "error": True})
    finally:
        sys.stdout.flush()
        sys.stderr.flush()
        sys.stdout, sys.stderr = out, err


def handle_input_change(field_id, text, old_text, cursor, char_w, line_h, pad_l, pad_t):
    field = _active_fields.get(field_id)
    if field is None:
        field = JavaEditTextBoldCursor(field_id)
        _active_fields[field_id] = field

    field.update_state(text, cursor, char_w, line_h, pad_l, pad_t)
    field.needs_anim = False

    call = MethodHookParam(field, [object()])
    for h in _active_method_hooks:
        if "EditTextBoldCursor" in h.target_class and h.method_name == "onDraw":
            if h.before:
                try:
                    h.before(call)
                except Exception as e:
                    send_ipc({"op": "log", "plugin": "hook", "text": traceback.format_exc(), "error": True})

    emit_anim_frame(field)


def handle_input_step(field_id):
    field = _active_fields.get(field_id)
    if field is None:
        return
    field.needs_anim = False
    call = MethodHookParam(field, [object()])
    for h in _active_method_hooks:
        if "EditTextBoldCursor" in h.target_class and h.method_name == "onDraw":
            if h.before:
                try:
                    h.before(call)
                except Exception as e:
                    send_ipc({"op": "log", "plugin": "hook", "text": traceback.format_exc(), "error": True})
    emit_anim_frame(field)


def emit_anim_frame(field):
    sparks = []
    cursor_x = 0.0
    cursor_y = 0.0
    cursor_h = 0.0
    cursor_active = not field.cursor_visible

    for d in list(field.overlay.drawables):
        b = d.bounds
        w = b[2] - b[0]
        h = b[3] - b[1]
        if getattr(d, 'shape', 0) == JavaGradientDrawable.RECTANGLE or w <= 4:
            cursor_x = b[0]
            cursor_y = b[1]
            cursor_h = h
            cursor_active = True
            continue
        cx = b[0] + w * 0.5
        cy = b[1] + h * 0.5
        r = max(1.0, max(w, h) * 0.5)
        color = d.color
        if color < 0:
            color = (1 << 32) + color
        alpha = float(d.alpha) / 255.0
        sparks.append({"x": cx, "y": cy, "r": r, "color": color, "alpha": alpha})

    cursor_info = {
        "visible": field.cursor_visible,
        "active": cursor_active,
        "x": cursor_x,
        "y": cursor_y,
        "height": cursor_h
    }
    send_ipc({
        "op": "anim_frame",
        "field_id": field.field_id,
        "active": field.needs_anim or len(sparks) > 0 or cursor_active,
        "sparks": sparks,
        "cursor": cursor_info
    })


def main():
    global _hooks_enabled, _current_activity
    send_ipc({"op": "ready", "python_version": sys.version})

    while True:
        raw = sys.stdin.readline()
        if not raw:
            break
        raw = raw.strip()
        if not raw:
            continue
        try:
            msg = json.loads(raw)
        except Exception:
            continue

        cmd = msg.get("cmd")
        if cmd == "init":
            _hooks_enabled = bool(msg.get("hooks_enabled", True))
            plugins = msg.get("plugins", [])
            for p in plugins:
                run_plugin(p["id"], p["name"], p["folder"], p.get("prefs"))

        elif cmd == "run":
            run_plugin(msg["id"], msg["name"], msg["folder"], msg.get("prefs"))

        elif cmd == "setting":
            p_id = msg.get("plugin")
            k = msg.get("key")
            v = msg.get("value")
            p = _plugins.get(p_id)
            if p:
                p._prefs[k] = v
                action = p._actions.get(k)
                if action:
                    try:
                        action()
                    except Exception:
                        p.error(traceback.format_exc())
                for cb in list(p._on_settings):
                    try:
                        cb(k, v)
                    except Exception:
                        p.error(traceback.format_exc())

        elif cmd == "send":
            text = msg.get("text", "")
            chat_id = msg.get("chat_id", 0)
            req_id = msg.get("req_id")
            cancelled = False
            for p in list(_plugins.values()):
                p._cancel_send = False
                for cb in list(p._on_send):
                    try:
                        import inspect
                        sig = inspect.signature(cb)
                        params = list(sig.parameters.values())
                        if len(params) >= 2:
                            res = cb(text, chat_id)
                        else:
                            res = cb(text)
                        if res is False or p._cancel_send:
                            cancelled = True
                            break
                        if isinstance(res, str):
                            text = res
                    except Exception:
                        p.error(traceback.format_exc())
                if cancelled:
                    break
            send_ipc({"op": "send_res", "req_id": req_id, "text": text, "cancelled": cancelled})

        elif cmd == "chat_opened":
            chat_id = msg.get("chat_id", 0)
            w = int(msg.get("width", 1200))
            h = int(msg.get("height", 800))
            activity = JavaActivity(chat_id, w, h)
            _current_activity = activity
            for p in list(_plugins.values()):
                for cb in list(p._on_chat_opened):
                    try:
                        import inspect
                        sig = inspect.signature(cb)
                        params = list(sig.parameters.values())
                        if len(params) >= 1:
                            cb(activity)
                        else:
                            cb()
                    except Exception:
                        p.error(traceback.format_exc())

        elif cmd == "chat_resize":
            w = int(msg.get("width", 1200))
            h = int(msg.get("height", 800))
            if _current_activity is not None:
                _current_activity._root._width = w
                _current_activity._root._height = h

        elif cmd == "input_change":
            handle_input_change(
                msg.get("field_id", "default"),
                msg.get("text", ""),
                msg.get("old_text", ""),
                int(msg.get("cursor", 0)),
                float(msg.get("char_w", 9.0)),
                float(msg.get("line_h", 20.0)),
                int(msg.get("pad_l", 6)),
                int(msg.get("pad_t", 4)),
            )

        elif cmd == "input_step":
            handle_input_step(msg.get("field_id", "default"))


if __name__ == "__main__":
    main()
)py";

} // namespace Margy::Plugins
