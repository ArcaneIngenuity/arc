![arc logo](https://cloud.githubusercontent.com/assets/7487822/10715923/e2e9d2e2-7b2f-11e5-84b4-de43f232d061.gif)

**Clone this repository** rather than "Download ZIP" or else you will lack submodule dependencies!

Refer to the [API documentation](http://arcaneingenuity.github.io/arc/).

Arc is a lightweight application framework for realtime apps, games, simulations, and rich user experiences, based on a powerful configuration / [data-driven](https://en.wikipedia.org/wiki/Data-driven_programming), [SOC](https://en.wikipedia.org/wiki/Separation_of_concerns) architecture.

Arc has been produced using several languages / environments, maximising flexibility. It ties it into nearly any timing, input, or rendering approach / engine. It has been tested in conjunction with Android, Windows, Linux, GLFW, EGL, OpenGL, WebGL, GSAP, ENet, and in terms of language/platform, Java, ActionScript, **JS (current)** and **C (current)**.

Arc separates your application into Model, View and Controller aspects, clarifying purpose in code while traditional gameloop-based architecture remains undisturbed. A simple but powerful configurative approach lets you set up your realtime MVC application structure in minutes, and provides ongoing value through its capacity to include nested mini-configurations in the appropriate App, Controller or View, allowing the user to specfy all application data, as well as one-off object creations, on one place, easily. MVC benefits include data-informed logic design, runtime recoupling (Ctrls, Views), simplified code review due to good separation of concerns, the ability to write new Views without touching existing game/simulation/business logic.