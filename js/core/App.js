App = function(id) //final
{
	this.DEBUG = true;

	this.id = id; //(JS only) for multi-app pages
	
	var phaser = this.phaser = new Phaser(this);
	var services = this.services = new ServiceHub(this);
	var input = this.input = new InputHub(); //set from outside
	this.pointer = undefined;
	this.display = new Display();
	this.timer = undefined; //set from outside
	this.model = undefined; //optional app-wide model
	this.view = undefined; //used when individual disjunction apps are used as modules
	//abstraction of the device used to perform screen pointing - encapsulates hierarchical transformation of device coordinates to focused view's own coordinate system
	
	this.setPointer = function(deviceIndex, xChannelIndex, yChannelIndex, selectIndex) //final
	{
		var pointer = this.pointer = new Pointer();
		pointer.device = this.input.array[deviceIndex];
		pointer.xChannel = pointer.device.channels[xChannelIndex];
		pointer.yChannel = pointer.device.channels[yChannelIndex];
		pointer.selectChannel = pointer.device.channels[selectIndex];
	}
	
	this.dispose = function() //final
	{
		phaser.dispose();
		input.dispose();
		services.dispose();
	}
	
	this.update = function(deltaSec) //final
	{
		input.poll();
		phaser.update(deltaSec);
		input.flush();
	}
}