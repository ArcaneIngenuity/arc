MainModel = function()
{
	dj.core.Model.call(this); //extend base framework class

	//TODO enter model members specific to this Phase
	//this.myData = 'data';
	this.monsters = [];
	
	this.arenaWidth = undefined;
	this.arenaHeight = undefined;
};

MainModel.prototype = Object.create(dj.core.Model.prototype);
MainModel.prototype.constructor = MainModel;

//TODO enter any enum constants specific to this Model
MY_CONSTANT = 0;