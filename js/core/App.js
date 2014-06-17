App = function() //(abstract)
{
	this.DEBUG = true;

	var phaser = this.phaser = new Phaser(this);
	var services = this.services = [];
	var input = this.input = new InputDeviceManager(); //set from outside
	this.pointer = new Pointer();
	this.timer = undefined; //set from outside
	this.model = undefined; //optional app-wide model
	//abstraction of the device used to perform screen pointing - encapsulates hierarchical transformation of device coordinates to focused view's own coordinate system
	
	this.setPointerDevice = function(deviceIndex, xChannelIndex, yChannelIndex, selectorIndex)
	{
		console.log('?');
		if (deviceIndex === undefined || xChannelIndex === undefined || yChannelIndex === undefined)
			throw "Error: setPointingDevice() requires a device index, and (x, y) channel indices as arguments.";
		
		this.pointer.xChannelIndex = xChannelIndex;
		this.pointer.yChannelIndex = yChannelIndex;
		this.pointer.selectorIndex = selectorIndex;
		this.pointer.device = this.input.devices[deviceIndex];
	}
	
	this.dispose = function()
	{
		phaser.dispose();
		input.dispose();
		//serviceManager.dispose();
		for (var name in this.services)
			services[name].dispose();
	}
	
	this.update = function(deltaSec)
	{
		input.poll();
		phaser.update(deltaSec);
		input.flush();
	}
}