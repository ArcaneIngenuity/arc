MainModel = function()
{
	Model.call(this); //extend base framework class
}; 

MainModel.prototype = Object.create(Model.prototype);
MainModel.prototype.constructor = MainModel;