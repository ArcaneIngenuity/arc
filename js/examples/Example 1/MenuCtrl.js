var menuCtrl = new Ctrl();
menuCtrl.start = function()
{
	console.log('start menu ctrl');
	var r = Math.ceil(Math.random() * 5);
	this.model.randomText = "Random Menu Text #" + r;
};

menuCtrl.update = function()
{
	//console.log(this)
	var app = this.app;
	
	if (app.model.hitOption)
	{
		//console.log('?')
		app.phaser.change(app.model.hitOption);
		app.model.hitOption = undefined; //reset the value
	}
};