Service = function()
{
	Disjunction.Core.JournalKeeper.call(this);
}

Service.prototype = Object.create(Disjunction.Core.JournalKeeper.prototype);
Service.prototype.constructor = Service;