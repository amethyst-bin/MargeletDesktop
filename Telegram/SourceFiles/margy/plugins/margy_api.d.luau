--!strict
-- Definition for Margy Plugin API (@margy)

export type CancelHandle = {
	cancel: () -> (),
}

export type SettingRow = {
	kind: string,
	key: string?,
	title: string,
	default: any?,
	options: {string}?,
	about: string?,
	danger: boolean?,
}

export type HookParam = {
	thisObject: any,
	args: {any},
	getResult: () -> any,
	setResult: (any) -> (),
}

export type MargyAPI = {
	id: string,
	name: string,
	folder: string,

	-- Logging
	log: (...any) -> (),
	error: (...any) -> (),

	-- Threading and Timers
	ui: (call: () -> (), delayMs: number?) -> CancelHandle?,
	every: (intervalMs: number, call: () -> ()) -> CancelHandle,
	cancel: (handle: CancelHandle?) -> boolean?,
	background: (call: () -> ()) -> (),

	-- User interface and Notifications
	toast: (text: string) -> (),
	activity: () -> any,
	window: (title: string, view: any) -> (),
	color: (argb: number) -> number,

	-- Persistent Preferences
	get: (key: string, fallback: string?) -> string?,
	set: (key: string, value: string?) -> (),
	flag: (key: string, fallback: boolean?) -> boolean,

	-- Messaging and Network
	send: (chatId: number | string, text: string) -> (),
	fetch: (url: string, callback: (response: string?) -> ()) -> (),

	-- Dynamic Settings UI
	header: (title: string) -> SettingRow,
	note: (text: string) -> SettingRow,
	switch: (key: string, title: string, default: boolean?, about: string?) -> SettingRow,
	text: (key: string, title: string, default: string?, about: string?) -> SettingRow,
	choice: (key: string, title: string, options: {string}, default: string?) -> SettingRow,
	action: (title: string, callback: () -> (), danger: boolean?) -> SettingRow,
	settings: (...SettingRow) -> (),
	on_settings: (callback: (key: string, value: string) -> ()) -> (),

	-- Event Subscriptions
	on_chat_opened: (callback: (chatId: any) -> ()) -> (),
	on_send: (callback: (text: string) -> string | boolean | nil) -> (),
	on_send_photo: (callback: (caption: string) -> string | boolean | nil) -> (),
	on_message: (callback: (text: string, dialogId: number, messageId: number, out: boolean) -> ()) -> (),
	on_deleted: (callback: (messageIds: {number}, chat: any) -> ()) -> (),
	on_pin: (callback: (chat: any, pin: boolean) -> boolean | nil) -> (),
	on_request: (callback: (request: any) -> any) -> (),
	on_answer: (callback: (request: any, response: any, error: any) -> any) -> (),
	on_update: (callback: (update: any) -> any) -> (),

	-- Menu Integrations
	button: (title: string, callback: (chat: any) -> ()) -> (),
	menu: (where: "chat" | "profile" | "message" | "drawer", title: string, callback: (...any) -> ()) -> (),
	pick_file: (callback: (path: string?) -> (), mimeTypes: string?) -> (),

	-- Method Hooking
	hooks_work: () -> boolean,
	hooks_why: () -> string,
	hook: (targetClass: string, methodName: string, before: ((HookParam) -> ())?, after: ((HookParam) -> ())?, args: {string}?) -> boolean,
}

return {}
