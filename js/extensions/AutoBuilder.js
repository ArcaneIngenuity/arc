//TODO all instances of 'window' should be replaced with 'dj' if  we separate things out
//TODO documentation saying what attributes (e.g. onfocus) not to implement when using this class!
AutoBuilder = function()
{
	this.tabIndex = undefined;
	
	this.buildFrom = function(domContainer)
	{
		this.addPhaseViews(domContainer);
	}
	
	//construct disjunction View tree (roughly) by DOM layout - do not include anything that does not have a classname
	this.addChildViews = function(view, element)//, parentDomRect)
	{
		for (var a = 0; a < element.children.length; a++)
		{
			var childElement = element.children[a];
			var classNamesJoined = childElement.className;
			
			var childView;
			if (classNamesJoined.length > 0)
			{
				var classNames = classNamesJoined.split(' ');
				var className = classNames[0];//[b];
				console.log(className);
				
				var ViewClass = window[className+'View'];
				if (ViewClass)
				{
					childView = new ViewClass();
					childView.domElement = childElement;
					childView.name = className;
					view.addChild(childView);
					console.log(view, childView);
					this.prepareElement(childElement, childView);
				}
			}
			
			/*
			var domRect = childElement.getBoundingClientRect();
			//console.log(domRect, 'for', element);
			
			childView.bounds = new Box2();
			childView.bounds.x0 = Math.floor(domRect.left - parentDomRect.left);
			childView.bounds.y0 = Math.floor(domRect.top - parentDomRect.top);
			childView.bounds.setWidth(domRect.width);
			childView.bounds.setHeight(domRect.height);
			*/
			//recurse
			if (childElement.children.length > 0)
				this.addChildViews(childView ? childView : view, childElement);//, domRect); //conditional will skip DOM tree levels that don't have a related View
			//else	
			//	childView.makeLeaf(); //if no DOM children, set View children array undefined
			

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
				this.tabIndex = 0;
				
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

						
						console.log(model, view, ctrl);
						this.prepareElement(element, view);

						view.domElement = element;
						var domRect = element.getBoundingClientRect();
						view.bounds.x0 = Math.floor(domRect.left);
						view.bounds.y0 = Math.floor(domRect.top);
						view.bounds.setWidth(domRect.width);
						view.bounds.setHeight(domRect.height);
						app.phaser.register(new Phase(className, model, view, ctrl));
						if (element.children.length > 0)
							this.addChildViews(view, element, domRect);
						//else	
						//	view.makeLeaf(); //if no DOM children, set View children array undefined
						
						
						
						console.log(view);
					}
				}
			}
		}
	}
	
	this.prepareElement = function(element, view)
	{
		element.view = view; // bind the view
		element.onfocus = function()
		{
			this.view.takeFocus();
			console.dir(this, this.getBoundingClientRect());
		}
		//{this.view.wasFocused = this.view.isFocused; this.view.isFocused = true;};
		//element.onblur = function() //{this.view.wasFocused = this.view.isFocused; this.view.isFocused = false;};
		//element.onmousedown = "app.phaser.phase.focus = "
		if (element.tabIndex == -1) //only add if not specified in markup
		{
			console.log(this);
			this.tabIndex++;
			element.tabIndex = this.tabIndex;
		}
	}
}