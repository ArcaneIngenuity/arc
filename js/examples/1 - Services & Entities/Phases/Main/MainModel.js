MainModel = function()
{
	DJ.Core.Model.call(this); //extend base framework class

	this.monsters = [];
	
	this.arenaWidth = undefined;
	this.arenaHeight = undefined;
};

MainModel.prototype = Object.create(DJ.Core.Model.prototype);
MainModel.prototype.constructor = MainModel;