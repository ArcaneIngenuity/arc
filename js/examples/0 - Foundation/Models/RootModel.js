Foundation.RootModel = function()
{
	Model.call(this); //extend base framework class

	//add data members here
	this.input = '';
}; 

Foundation.RootModel.prototype = Object.create(Model.prototype);
Foundation.RootModel.prototype.constructor = Foundation.RootModel;