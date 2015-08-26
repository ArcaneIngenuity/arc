Arc is a lightweight applications development framework for realtime apps, specifically geared for games, simulations, and rich user experiences.

Arc is platform-agnostic. You should be able to tie it into any timing, input, or rendering approach, or any engine you please. (Tested so far with GLFW, OpenGL, Android, Windows and Linux, using the closed-source Orb engine, and the ENet networking library.)

Arc is best suited where you are writing the control loop of your application yourself (typical in traditional games development), rather than where you intend a primarily-event based application structure; but methods are available for adapting event-based aspects to arc's loop-based approach (e.g. GLFW or SDL input).

Arc separates each application into Model, %View and Controller aspects. Some game developers are put off by these terms, seeing them as concepts that belong in the business world. In fact, separating logic this way clarifies purpose in code, even as standard loop-based game architecture remains fundamentally unchanged. MVC is also desirable in instances where a pure data model (no methods / functions) is ideal, either for ease of code review or for state serialisation (refer to Android's saved application state and typical packet construction for TCP and UDP networking).

Arc (formerly known as disjunction) has been in development since 2011 in four different languages, some of which are no longer supported: C, JavaScript, Java, ActionScript. Each of these environments provides very different methods of access to core timing, input and rendering. Consequently, arc is exceptionally flexible.

API documentation is available [here](http://arcaneingenuity.github.io/arc/).
