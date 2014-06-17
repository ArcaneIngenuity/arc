//Global Pointer data:
//holds information about the pointer device
//knows which ONE element is currently pointed to, out of the entire tree
//checks if that target has changed
//can determine whether selected and release are same

Pointer = function() //ABSTRACT / INTERFACE
{
	this.device = undefined;
	this.xChannelIndex = undefined;
	this.yChannelIndex = undefined;
	this.selectorIndex = undefined; //usually 0 for touch or mouse
	this.isSelected = false;
	this.wasSelected = false;
	
	this.target = undefined;
	this.targetLast = undefined;
	this.targetSelected = undefined;
	this.targetReleased = undefined;
	this.targetPosition;// = new Point2(); //local position of pointer within pointed element
	this.position = new Point2(); //local position of pointer within pointed element
	this.entered = undefined;
	this.exited = undefined;
	
	//dragging has to be done OUTSIDE of the target element, due to the fact that we are moving it within its parent... and also that moving outside of its own bounds would otherwise break the drag.
	this.dragging = undefined;
	
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
		var device = this.device;
		this.dragging.bounds.x0 += device.channels[this.xChannelIndex].delta;
		this.dragging.bounds.x1 += device.channels[this.xChannelIndex].delta;
		this.dragging.bounds.y0 += device.channels[this.yChannelIndex].delta;
		this.dragging.bounds.y1 += device.channels[this.yChannelIndex].delta;
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
		//var channelDelta = this.device.channels[this.selectorIndex].delta;
		var channelValue = this.device.channels[this.selectorIndex].value;
		this.isSelected = channelValue > 0;
		
		//console.log(')))',this.isSelected, this.wasSelected);
		/*
		if (channelDelta > 0)
		{
			this.isSelected = true;
		}
		else if (channelDelta < 0)
		{
			this.isSelected = false;
		}
		*/
	}
	
	//transform from parent into child coordinates
	//after all transformations down the hierarchy, the point is in the focused View's coordinate system.
	//TODO later: scaling; custom transform centre

	
	this.storePositionWorld = function()
	{
		var device = this.device;
		this.position.x = device.channels[this.xChannelIndex].value;
		this.position.y = device.channels[this.yChannelIndex].value;
		if (device.channels[this.xChannelIndex].delta != 0)
		{
			//console.log('WORLD', this.position.x, this.position.y);
			//console.log('TGT  ', this.targetPosition);
			
		}
	}
	
	
	//TODO should really run through local Controls (a leaf view), and this function should return which one the pointer is in, so we can run a simple switch on the result.
	//note, argument bounds to be in same coordinate system as pointer itself. specific geometry is optional.
	this.isIn = function(view) //intersects
	{	
		return view === this.target;
	}
}