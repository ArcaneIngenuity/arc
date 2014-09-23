Disjunction.Extensions.DragTimeline = function(app, phase, model, view, ctrl)
{
	Timeline.call(this, app, phase, model, view, ctrl)
	this.grabOffset = undefined;
	
	this.isToStart = function()
	{
		var pointer = this.app.pointer;
		if (pointer.selectChannel.delta > 0)
		{
			
		}
	}
	
	this.isToFinish = function()
	{
		var pointer = this.app.pointer;
		return pointer.selectChannel.delta < 0;
	}
	
	this.start = function()
	{
		var model = this.model;
		var view = this.view;
		var pointer = this.app.pointer;
	
		//this.grabOffset = ;
	}
	
	this.input = function(deltaSec)
	{
		/*
		var model = this.model;
		var view = this.view;
		var mouse = view.app.input.devices[INPUT_MOUSE];
		var keyboard = view.app.input.devices[INPUT_KEYBOARD];
		
		if (mouse.channels[MOUSE_BUTTON_LEFT].value) //ongoing
		{
			var mouseX = mouse.channels[MOUSE_X].value;
			var mouseY = mouse.channels[MOUSE_Y].value;
			if (mouseX.delta != 0 || mouseY.delta != 0) //don't raycast unless the mouse has moved
			{
				var intersects = view.pick(mouseX, mouseY, [view.room], false);
				if (intersects.length > 0)
				{
					var intersectionPoint = intersects[0].point;
					view.room.worldToLocal(intersectionPoint);
					model.blockEnd = intersectionPoint;
					console.log('CREATING');
				}
			}
		}
		*/
	}
	
	this.stop = function()
	{
		/*
		var model = this.model;
		var view = this.view;
		var mouse = view.app.input.devices[INPUT_MOUSE];
		var keyboard = view.app.input.devices[INPUT_KEYBOARD];
	
		//view.startMarker.visible = view.endMarker.visible = false;
		view.block = undefined;
		model.block = model.blockStart = model.blockStartLast = model.blockEnd = view.block = undefined;
		
		view.blockCreationIndicator.style.visibility = "hidden";
		*/
		view.mode = undefined;
	}

	this.simulate = function(deltaSec)
	{
		var model = this.model;
		model.x += model.xVelocity * deltaSec;
		model.y += model.yVelocity * deltaSec;
		model.xVelocity *= 0.98;
		model.yVelocity *= 0.98;
		//console.log(model);
	}
}

Disjunction.Extensions.DragTimeline.prototype = Object.create(Disjunction.Core.prototype);
Disjunction.Extensions.DragTimeline.prototype.constructor = Disjunction.Extensions.DragTimeline;

if (disjunction.WINDOW_CLASSES) 
	window.DragTimeline = Disjunction.Extensions.DragTimeline;