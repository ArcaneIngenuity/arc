Phase = function(name, model, view, ctrl) //(abstract) Updatable, Disposable
{
	/** Must be defined before it can be set into the manager's map. should be unique. */
	this.name  = name;
	
	this.model = model; 
	this.view  = view; //root

	this.ctrl  = ctrl;
	this.view.model = model; //inject
	this.ctrl.model = model; //inject
	
	this.app = undefined;
	
	this.focus = view; //focus on the root till changed

                       
	this.setApp = function(app)
	{
		this.app = app;                    
		this.ctrl.app = app;
		this.view.app = app;
		this.view.root = this.view;
		setAppRecurse(this.view);
	}
	
	var setAppRecurse = function(view)
	{
		var numChildren = view.children.length;
		for (var i = 0; i < numChildren; i++)
		{
			var viewChild = view.children[i];
			viewChild.app = this.app;
			viewChild.root = this.root;
			
			setAppRecurse(viewChild);
		}
	}

	this.start = function() //final
	{
		this.ctrl.start();
		this.view.start();
	}
	
	this.finish = function() //final
	{
		this.ctrl.finish();
		this.view.finish();
	}
	
	this.dispose = function() //final
	{
		ctrl.dispose();
		view.dispose();
	}
	
	
	this.update = function(deltaSec) //final
	{
		//having global input via args, use it to refresh focus if necessary...
		//1. walk down chain from root to current focus, using custom focus logic to determine who now has focus
		//--we cannot rely on child views being within the bounds of a parent view. For this reason, seeking top-down may as well be done, since all elements may geometrically separate anyway, and top-down is conceptually simpler than focus-to-root.
		//--we must in any case search the whole tree since we cannot assume exclusion of ancestors during interaction -- this is specific to user implementation
		//--TODO it is possible that multiple foci could be found at the same depth, if two views were overlapping. This is where depth/z-order comes in, even for 2D picking.
		//--TODO if at focus (deepest) and explicit escape, change focus upward -- then process that ancestor as the focus this frame; walk upward again in case a parent e.g. highlights based on whether a child was found and focused.
		//--TODO we could terminate the search early if every child was guaranteed to be fully contained in its parent -- maybe set up a switch for this.
		var view = this.view;
		var ctrl = this.ctrl;
		if (view.enabled) //root enabled
		{
			//TODO need to get pointer position transformed for each unique path down tree to relevant element 
			//---- either we do this BFS such that we save on calculations
			//---- or we just restore the world position and work with it each time.
			var pointer = this.app.pointer;
			pointer.storePositionWorld(); //screen
			
			
			//console.log(pointer.isSelected);
			
			if (pointer.position) //often undefined for touch interfaces
			{
				pointer.setTargetLast();
				
				//1. determine which views pointer is in (exhaustively because parents do not necessarily geometrically contain children)
				var intersections = [];
				this.getIntersected(view, pointer.position, intersections); //in root
				
				//2. Select intersection that is last rendered
				//console.log('intersections', intersections);
				pointer.target = intersections.length > 0 ? intersections[intersections.length-1] : undefined;
				//console.log(intersections[intersections.length-1] , pointer.target, pointer.targetLast);
				if (pointer.hasChangedTarget())
				{
					if (pointer.targetLast) pointer.targetLast.onExited();
					if (pointer.target)		pointer.target.onEntered();

				}
				pointer.updateSelectedness();

			}
			
			
		}
		if (pointer.isSelected)	this.focus = pointer.target;
		
		//console.log('focus', this.focus);
		//2. pass input specifically to the focused view to process
		this.focus.input(deltaSec); //only one view gets UI input processed
		if (pointer.dragging) pointer.progressDrag();
		ctrl.update(deltaSec); //3.
		view.outputRecurse(deltaSec); //4. all views get rendered
		
		//TODO almost looks like those method signatures should be func(time, input) -- since these are the two things that change since last tick. Time should consist of value and delta.
	}
	
	//adds to the intersected array in BFS order
	this.getIntersected = function(viewRoot, pointerPosition, intersections)
	{	
		//console.log(viewRoot.children.length);
		var unvisited = [viewRoot];
		
		while (unvisited.length > 0)
		{
			var view = unvisited.shift();
			
			if (view.enabled)
			{
				//add children if enabled
				//console.log(view);
				for (var i = 0; i < view.children.length; i++)
				{
					var childView = view.children[i];
					if (childView.enabled)
					{
						//console.log('en', i)
						unvisited.push(childView);
						//console.log('////', unvisited.length);
					}
					//this.getIntersected(childView, pointerPosition, intersections);
				}
				
				//transform position to local
				var pointerTargetPosition = this.app.pointer.targetPosition = pointerPosition.clone();

				view.fromWorld(pointerTargetPosition);

				//check intersection
				var inBounds = view.bounds.contains(pointerTargetPosition);
				var inGeometry = view.geometry ? view.geometry.intersects(pointerTargetPosition) : true;
				//if (view instanceof PanelView)
				//console.log(/*pointerPosition, */pointerTargetPosition, view.bounds);//, inBounds, inGeometry)
					
				if (inBounds && inGeometry)
				{
					intersections.push(view);
				}
			}
		}
	}
	
	this.changeFocus = function(view)
	{
		console.log('phase.changeFocus');
		this.focus.loseFocus();
		this.focus = view;
		this.focus.gainFocus();
	}
};