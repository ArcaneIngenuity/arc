//TODO all instances of 'window' should be replaced with 'dj' if  we separate things out

ViewParser = function()
{
	this.parse = function(container)
	{
		this.addPhaseViews(container);
	}
	//construct views by DOM layout
	//TODO should be in an extension script, included here
	this.addChildViews = function(view, element, parentDomRect)
	{
		for (var a = 0; a < element.children.length; a++)
		{
			var childElement = element.children[a];
			var classNamesJoined = childElement.className;
			if (classNamesJoined.length > 0)
			{
				var classNames = classNamesJoined.split(' ');
				for (var b = 0; b < classNames.length; b++)
				{
					var className = classNames[b];
					var childView;
					console.log(className);
					var ViewClass = window[className+'View'];
					if (ViewClass)
						childView = new ViewClass();

					//this.addChildViews(childView, childElement);
					
					childView.domElement = childElement;
					var domRect = childElement.getBoundingClientRect();
					console.log(domRect, 'for', element);
					childView.bounds.x0 = Math.floor(domRect.left - parentDomRect.left);
					childView.bounds.y0 = Math.floor(domRect.top - parentDomRect.top);
					childView.bounds.setWidth(domRect.width);
					childView.bounds.setHeight(domRect.height);
						
					view.addChild(childView);
				}
			}
		}
	}
	
	this.addPhaseViews = function(container)
	{
		for (var a = 0; a < container.children.length; a++)
		{
			var element = container.children[a];
			if (element.tagName.toUpperCase() === 'DIV')
			{
				//assume phase
				var classNamesJoined = element.className;
				console.log(classNamesJoined);
				if (classNamesJoined.length > 0)
				{
					var classNames = classNamesJoined.split(' ');
					for (var b = 0; b < classNames.length; b++)
					{
						var className = classNames[b];
						var model, view, ctrl;
					
						var ModelClass = window[className+'Model'];
						if (ModelClass)
							model = new ModelClass();
							
						var ViewClass = window[className+'View'];
						if (ViewClass)
							view = new ViewClass();
							
						var CtrlClass = window[className+'Ctrl'];
						if (CtrlClass)
							ctrl = new CtrlClass();
							
						

						view.domElement = element;
						var domRect = element.getBoundingClientRect();
						console.log(domRect, 'for', element);
						view.bounds.x0 = Math.floor(domRect.left);
						view.bounds.y0 = Math.floor(domRect.top);
						view.bounds.setWidth(domRect.width);
						view.bounds.setHeight(domRect.height);
						
						//console.log(className, model, view, ctrl);
						//console.log( '!!', domRect);
						//console.log( '??', element.offsetLeft);
						//console.log(view.bounds);
						
						this.addChildViews(view, element, domRect);
						
						app.phaser.register(new Phase(className, model, view, ctrl));
					}
				}
			}
		}
	}
}