var menuView = new View();
menuView.start = function()
{
	//...MenuView initialisation here... (called when MenuPhase is entered, after MenuCtrl.start)
	console.log('starting MenuMiew...');
	
	//ordinarily we'd pre-assign these as part of the View, to avoid calling getElementById() every frame
	var menuDOM = document.getElementById('Menu');
	menuDOM.style.display = 'block';
	
	var randomTextDOM = document.getElementById('RandomText');
	randomTextDOM.innerHTML = this.model.randomText;
	console.log(randomTextDOM, app.model);
}

menuView.finish = function()
{
	var menuDOM = document.getElementById('Menu');
	menuDOM.style.display = 'none';
}

menuView.update = function()
{
	//nothing
}

//helper function: when using the DOM, we must rely on its subsystem for interactions.
menuView.onExitClicked = function()
{
	//...MenuView cleanup here... (called when MenuPhase is exited)
	console.log('exiting MenuView...');

	this.app.model.hitOption = 'exit'
	
}