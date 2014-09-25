//Global Pointer data:
//holds information about the pointer device
//knows which ONE element is currently pointed to, out of the entire tree (target, not focus)
//checks if that target has changed
//abstracts concepts of pointer to be device-agnostic

Disjunction.Core.Pointer = function() //ABSTRACT / INTERFACE
{
	this.device = undefined;
	this.isSelected = false;
	this.wasSelected = false;
	
	this.target = undefined;
	this.targetLast = undefined;
	this.targetSelected = undefined;
	this.targetReleased = undefined;
	this.positionInTarget;// = new Point2(); //local position of pointer within pointed element
	this.position = new Disjunction.Core.Point2(); //world position of pointer within pointed element
	this.entered = undefined;
	this.exited = undefined;
	
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
	
	//see whether target we released over is same we selected over
	this.isReleasedSelectedTarget = function()
	{
		return targetSelected === targetReleased;
	}
	
	this.updateSelected = function() //TODO change for touch devices.
	{
		this.wasSelected = this.isSelected;
		if (this.selectChannel.delta > 0)
			this.isSelected = true;
		else if (this.selectChannel.delta < 0)
			this.isSelected = false;
		//...otherwise don't change select state.
	}
	
	this.findTarget = function(view)
	{
		//transform from parent into child coordinates
		//after all transformations down the hierarchy, the point is in the targeted View's coordinate system.
		//TODO later: scaling; custom transform centre
		var device = this.device;
		this.position.x = this.xChannel.value;
		this.position.y = this.yChannel.value;

		//TODO implement for 3 dimensions as well as 2.
		if (this.position.x !== undefined && this.position.y !== undefined) //often undefined for touch interfaces
		{
			if (this.intersectsView(view))
				this.target = view;
			
			while (this.target.children.length > 0)
			{
				var children = this.target.children;

				var targetCandidates = [];
				for (var i = 0; i < children.length; i++)
				{
					var child = children[i];
					if (this.intersectsView(child))
						targetCandidates.push(child);
				}
				
				//break ties by selecting the highest index (last rendered)
				if (targetCandidates.length > 0)
				{
					this.target = targetCandidates[targetCandidates.length - 1];
				}
				else
					break;
			}
			
			this.setTargetLast();
			
			if (this.target)
			{
				this.positionInTarget = this.position.clone();
				this.target.fromWorld(this.positionInTarget);
			}
		}
	}
	
	this.intersectsView = function(view)
	{
		var pointerPositionInTarget = this.positionInTarget = this.position.clone();
		//view.fromWorld(pointerPositionInTarget); //necessary even for root view, when it isn't a standard fullscreen, origin-positioned element.
		var inBounds = view.bounds.containsWorld(pointerPositionInTarget);
		var inGeometry = view.geometry ? view.geometry.intersects(pointerPositionInTarget) : true;
		return inBounds && inGeometry;
	}
};

if (disjunction.WINDOW_CLASSES) 
	window.Pointer = Disjunction.Core.Pointer;