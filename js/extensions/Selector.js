/**
 *	Selects element in an array by index or by object property.
 */
Selector = function(array, journal)
{
	this.array = array;
	this.journal = journal || new Journal(2, false);
	
	this.invalidate = function()
	{
		this.journal.index.update(-1);
	}
	
	this.start = function()
	{
		return this.select(0);
	}
	
	this.end = function()
	{
		return this.select(this.array.length - 1);
	}
	
	this.last = function()
	{
		return this.select(this.journal.last());
	}
	
	this.current = function()
	{
		return this.select(this.journal.current());
	}
	
	this.setArray = function(value)
	{
		array = this.array = value;
		this.invalidate();
	}
	
	this.select = function(index)
	{
		this.journal.update(index);
		return this.current();
	}
	
	this.onLast = function()
	{
		return this.journal.current() == this.array.length - 1;
	}
	
	this.isIndexValid = function(index)
	{
		return 0 <= index && index <= this.array.length - 1;
	}
	
	this.selectByKey = function(key, value)
	{
		this.journal.update(this.array.map(function(e) { return e[key]; }).indexOf(value));
		return this.current();
	}
}