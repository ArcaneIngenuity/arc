var menuCtrl = new Ctrl();
menuCtrl.start = function()
{
	//...MenuCtrl initialisation here... (called when MenuPhase is entered, before MenuView.start)
	console.log('starting MenuCtrl...');
	
	var r = Math.ceil(Math.random() * 5);
	this.model.randomText = "Random Menu Text #" + r;
};

menuCtrl.update = function()
{
	var app = this.app;
	
	if (app.model.hitOption)
	{
		app.phaser.change(app.model.hitOption);
		app.model.hitOption = undefined; //reset the value
	}
};