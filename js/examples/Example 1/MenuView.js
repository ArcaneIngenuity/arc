var menuView = new View();
menuView.start = function()
{
	console.log('start menu view');
	//ordinarily we'd cache these as part of the View
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
	this.app.model.hitOption = 'exit'
	console.log('exiting...');
}