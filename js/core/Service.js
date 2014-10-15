Service = function()
{
	Disjunction.Core.JournalKeeper.call(this);
	
	this.dispose = function()
	{
		//ABSTRACT
	}
}

Service.prototype = Object.create(Disjunction.Core.JournalKeeper.prototype);
Service.prototype.constructor = Service;