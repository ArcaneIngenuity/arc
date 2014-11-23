Foundation.RootCtrl = function(app, model)
{ 
	Ctrl.call(this, app, model); //extend base framework class
	
	this.isToStart = function()
	{
		return true;
	};
	
	this.isToStop = function()
	{
		return false; //for closing application from within
	};
	
	this.start = function()
	{
		return true;
	};
	
	this.stop = function()
	{
	};
	
	this.update = function()
	{
		var devices = disjunction.devices;
		//console.log(disjunction.devices.array)
		//get Pointer
		var pointer = disjunction.pointer;
		var keyboard = devices.array[DEVICE_KEYBOARD];
		
		//***TODO convert raw input to (App or Phase) Model state and View state.
		if (pointer.selectChannel.value)
		{
			model.input = 'pointer';
			console.log( 'pointer');
		}	
		else if (keyboard.channels[disjunction.constants.KEY_SPACE].value)
		{
			model.input = 'spacebar';
			console.log( 'spacebar' );
		}
		else
			model.input = '';
	};
	
	this.refresh = function()
	{
	};
};

Foundation.RootCtrl.prototype = Object.create(Ctrl.prototype);
Foundation.RootCtrl.prototype.constructor = Foundation.RootCtrl;