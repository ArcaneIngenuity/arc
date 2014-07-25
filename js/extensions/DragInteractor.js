DragInteractor = function(app, phase, model, view, ctrl)
{
	Interactor.call(this, app, phase, model, view, ctrl)
	this.grabOffset = undefined;
	
	this.hasInputStarted = function()
	{
		var pointer = this.app.pointer;
		return pointer.selectChannel.delta > 0;
	}
	
	this.hasInputFinished = function()
	{
		var pointer = this.app.pointer;
		return pointer.selectChannel.delta < 0;
	}
	
	this.inputStart = function()
	{
		var model = this.model;
		var view = this.view;
		var pointer = this.app.pointer;
	
		//this.grabOffset = ;
	}
	
	this.inputUpdate = function(deltaSec)
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
	
	this.inputFinish = function()
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

	this.ctrlUpdate = function(deltaSec)
	{
		/*
		var model = this.model;
		var ctrl = this.ctrl;
		//if (model.block) //ongoing - if there is a current block
		//{
		//this.updatePattern(bounds); //exactly what we're doing below
		
		var block = model.block;
		var bounds = block.bounds;
		var width = bounds.getWidth();
		var height = bounds.getHeight();
		
		block.xCountLast = block.xCount;
		block.yCountLast = block.yCount;
		block.xCount = Math.abs(Math.floor(width  / block.xSpacePerElement));
		block.yCount = Math.abs(Math.floor(height / block.ySpacePerElement));
		block.xCountDelta = block.xCount - block.xCountLast;
		block.yCountDelta = block.yCount - block.yCountLast;

		//TODO move into this class, out of ctrl?
		ctrl.alignBox2(bounds, model.blockStart, model.blockEnd);
		
		if (block.changedPattern())
		{
			model.block.modified = true;
		}
		//}
		*/
	}
}

DragInteractor.prototype = Object.create(Interactor.prototype);
DragInteractor.prototype.constructor = DragInteractor;