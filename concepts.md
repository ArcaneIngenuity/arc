 * Hub: contains and updates Apps - exists so we may run multiple Apps in parallel off the same timing mechanism (loop or callback).
 * App: contains and updates Ctrls (and thus models) and Views.
 * model: a generic object (user specified type) that holds logical world state.
 * View: a component that assists in rendering a discrete thing, like a game-world view, a menu or an inventory.
 * Ctrl: provides the basic logic that operates on (reads and writes) the model.
 * update/updatePost: For Ctrl, update within a given App update, before and after all that App's Views update, respectively. For View, update before or after that View's children, respectively.
 * initialise/dispose: attain resources and/or make allocations OR release resource / free allocations that are specific to this object (Hub, App, View, Ctrl); typically one-off operations at executable startup & shutdown.
 * start/stop: commence or cease updates (App, View, Ctrl; Hub is not included here as it is always updating).
 * resume/suspend: for when mandatory application context is regained or lost, typically due to a loss and subsequent restoration of the window (and thus rendering and input capability) or graphics context (Hub, App, View, Ctrl); naturally a stop is required before  a suspend, while a start is required after a resume.

update/updatePost, initialise/dispose, start/stop and suspend/resume are generally all recursive functions, irrespective of where they appear.
 