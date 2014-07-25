//all Bindable stuff for Model -- need not be extended, can be accessed as "static" in JS, i.e. from this object
Model = function()
{
	this.updateHistory = function()
	{
		//ABSTRACT
	}

	this.serialise = this.serialize = function()
	{
		//ABSTRACT
	}
	
	this.deserialise = this.deserialize = function()
	{
		//ABSTRACT
	}
}