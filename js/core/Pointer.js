//Global Pointer data:
//holds information about the pointer device
//knows which ONE element is currently pointed to, out of the entire tree (target, not focus)
//checks if that target has changed
//knows (by proxy) the focus 
//abstracts concepts of pointer to be device-agnostic

Pointer = function() //ABSTRACT / INTERFACE
{
	this.device = undefined;
	//this.xChannelIndex = undefined;
	//this.yChannelIndex = undefined;
	//this.selectorIndex = undefined; //usually 0 for touch or mouse
	this.isSelected = false;
	this.wasSelected = false;
	
	this.target = undefined;
	this.targetLast = undefined;
	this.targetSelected = undefined;
	this.targetReleased = undefined;
	this.positionInTarget;// = new Point2(); //local position of pointer within pointed element
	this.position = new Point2(); //world position of pointer within pointed element
	this.entered = undefined;
	this.exited = undefined;
	
	this.focus = undefined;
	this.focusLast = undefined;
	
	this.xChannel = undefined;
	this.yChannel = undefined;
	this.selectChannel = undefined;
	
	this.dragging = undefined;
	
	this.targetSupplied = undefined;
	
	this.startDragTarget = function()
	{
		this.dragging = this.target;
	}
	
	this.stopDragTarget = function()
	{
		this.dragging = undefined;
	}
	
	this.setTargetLast = function()
	{
		this.targetLast = this.target;
	}
	
	this.progressDrag = function()
	{
		this.dragging.bounds.x0 += this.xChannel.delta;
		this.dragging.bounds.x1 += this.xChannel.delta;
		this.dragging.bounds.y0 += this.yChannel.delta;
		this.dragging.bounds.y1 += this.yChannel.delta;
	}
	//generally called by view to do some animation such as border highlight
	this.hasChangedTarget = function()
	{
		return this.target !== this.targetLast;
	}
	
	this.hasChangedSelected = function()
	{
		return this.isSelected != this.wasSelected;
	}
	
	
	this.isReleasedSelectedTarget = function()
	{
		return targetSelected === targetReleased;
	}
	
	this.updateSelectedness = function() //TODO change for touch devices.
	{
		this.wasSelected = this.isSelected;
		if (this.selectChannel.delta > 0)
			this.isSelected = this.selectChannel.value > 0;
	}
	
	this.findTarget = function(view)
	{
		//transform from parent into child coordinates
		//after all transformations down the hierarchy, the point is in the focused View's coordinate system.
		//TODO later: scaling; custom transform centre
		var device = this.device;
		this.position.x = this.xChannel.value;
		this.position.y = this.yChannel.value;
		//console.log(device);
		if (this.position.x && this.position.y) //often undefined for touch interfaces
		{
			//1. determine which views pointer is in (exhaustively because parents do not necessarily geometrically contain children)
			var intersectedViews = [];
			this.getIntersectedViews(view, this.position, intersectedViews); //in root
			
			//2. Select as target that intersected view which is the last rendered
			this.setTargetLast();
			this.target = intersectedViews.length > 0 ? intersectedViews[intersectedViews.length-1] : undefined;
			
			//3. get position of pointer within target
			this.positionInTarget = this.position.clone();
			this.target.fromWorld(this.positionInTarget);
			//console.log('in')
		}
	}
	
	this.pollFocus = undefined; //function(phase)
	//{
		//SET THIS ON THE PHASE, to some external function (composition) 
		//TODO use app.input & pointer (as just updated) to determine focus
		//TODO need to tie in DOM-like approach to getting focus - keyboard
	//}
	
	//adds to the intersectedViews array in BFS order
	this.getIntersectedViews = function(viewRoot, pointerPosition, intersectedViews)
	{
		var unvisited = [viewRoot];
		
		while (unvisited.length > 0)
		{
			var view = unvisited.shift();
			
			if (view.enabled && view.children)
			{
				//expand (stackless) BFS queue. note that this assumes a child only ever belongs to one parent - we could use a set to be sure.
				for (var i = 0; i < view.children.length; i++)
				{
					var childView = view.children[i];
					if (childView.enabled)
					{
						unvisited.push(childView);
					}
				}
				
				//add view if intersected
				var pointerTargetPosition = this.positionInTarget = pointerPosition.clone();
				view.fromWorld(pointerTargetPosition);
				var inBounds = view.bounds.contains(pointerTargetPosition);
				var inGeometry = view.geometry ? view.geometry.intersects(pointerTargetPosition) : true;
				if (inBounds && inGeometry)
					intersectedViews.push(view);
			}
		}
	}
	
	//TODO should really run through local Controls (a leaf view), and this function should return which one the pointer is in, so we can run a simple switch on the result.
	//note, argument bounds to be in same coordinate system as pointer itself. specific geometry is optional.
	this.isIn = function(view) //intersects
	{	
		return view === this.target;
	}
}