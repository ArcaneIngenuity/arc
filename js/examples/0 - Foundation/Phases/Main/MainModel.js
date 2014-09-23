MainModel = function()
{
	Model.call(this); //extend base framework class

	//TODO enter model members specific to this Phase
	this.myData = 'data';
};

MainModel.prototype = Object.create(Disjunction.Core.Model.prototype);
MainModel.prototype.constructor = MainModel;

//TODO enter any enum constants specific to this Model
MY_CONSTANT = 0;