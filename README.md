![arc logo](https://cloud.githubusercontent.com/assets/7487822/10715923/e2e9d2e2-7b2f-11e5-84b4-de43f232d061.gif)

**Clone this repository** rather than "Download ZIP" or else you will lack submodule dependencies!

Refer to the [API documentation](http://arcaneingenuity.github.io/arc/).

Arc is a lightweight application framework for realtime apps, games, simulations, and rich user experiences, based on a powerful configuration / [data-driven](https://en.wikipedia.org/wiki/Data-driven_programming), [SOC](https://en.wikipedia.org/wiki/Separation_of_concerns) architecture.

Arc has been produced using several languages / environments, maximising its flexibility. It can tie it into nearly any timing, input, or rendering approach / engine. It has so far been tested in conjunction with Android, Windows, Linux, GLFW, EGL, OpenGL, WebGL, GSAP, ENet, and in terms of language/platform, Java, ActionScript, **JS (current)** and **C (current)**.

Arc separates your application into Model, View and Controller aspects, clarifying purpose in code while traditional gameloop-based architecture remains undisturbed. A simple but powerful configuration approach lets you set up your realtime MVC application structure in minutes, and provides ongoing value through its capacity to include nested mini-configurations in the appropriate Controller or View, allowing the framework user to specfy any and all application data, as well as one-off object creations, through arc.config with minimal fuss. Other benefits of MVC include data-driven design, ease of code review due to proper separation of concerns, the ability to write new Views without needing to touch _any_ existing game/simulation logic, the ability to start-stop / couple-decouple new Views at any time, and many more.