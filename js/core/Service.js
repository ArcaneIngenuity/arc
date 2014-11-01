Disjunction.Core.Service = function(model)
{
	this.model = model;
	
	this.models = []; /** Models belonging exclusively to this Service, to allow for their Journals to be updated. DO NOT add the injected model to this or it will be updated twice! */
	
	this.dispose = function()
	{
		//ABSTRACT
	}
}

if (disjunction.WINDOW_CLASSES) 
	window.Service = Disjunction.Core.Service;