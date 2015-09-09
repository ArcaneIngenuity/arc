Glossary
========

 * Hub: contains and updates Apps - exists so we may run multiple Apps in parallel off the same timing mechanism (loop or callback).
 * App: contains and updates Ctrls (and thus models) and Views.
 * model: a generic object (user specified type) that holds logical world state.
 * View: a component that assists in rendering a discrete thing or set of things, like a game-world view, a menu or an inventory.
 * Ctrl: provides the basic logic that operates on (reads and writes) the model.
 * update/updatePost: For Ctrl, update within a given App update, before and after all that App's Views update, respectively. For View, update before or after that View's children, respectively.
 * construct/destruct: allocates an instance of App, Ctrl, or View AND does framework-level initialise thereon, and deallocates an instance and does framework-level dispose thereon, respectively.
 * initialise/dispose: attain resources and/or make allocations AND release resource / free allocations that are specific to this object (Hub, App, View, Ctrl); typically one-off operations at executable startup & shutdown. _Resource Acquisition IS NOT Initialisation_ in arc's case: initialise for a given instrance is never called on construct of same instance, but rather at some point after; whereas dispose is always called on destruct of same instance.
 * start/stop: commence AND cease updates (App, View, Ctrl; Hub is not included here as it is always updating), respectively.
 * resume/suspend: for when mandatory application context is regained or lost, typically due to a loss and subsequent restoration of the window (and thus rendering and input capability) or graphics context (Hub, App, View, Ctrl); naturally a stop is required before  a suspend, while a start is required after a resume.

update/updatePost, initialise/dispose, start/stop and suspend/resume are generally all recursive functions, irrespective of where they appear. Exceptions are:

 * View_start()/View_stop(): Views must be started individually by Ctrl(s), and this is not recursive, so though stopping a parent View prevents all descendant Views from being updated, internally, descendants' "updating" flag remains as they were, so if they were updating then they will continue updating when their ancestor is View_start()ed again.
 