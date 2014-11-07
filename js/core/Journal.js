/**
 *	@const @function @constructs Disjunction.Core.Journal
 *	@param {Array} states The states array.
 *	@param {Array} deltas The deltas array.
 */
Disjunction.Core.Journal = function(states, deltas)
{
	this.states = states;
	this.deltas = deltas;
	
	this.stateLocked = false;
	this.deltaLocked = false;
};

/**
 *	Unlock the Journal.
 *	@const @function
 */
Disjunction.Core.Journal.prototype.unlock = function() //framework use only!
{
	this.stateLocked = this.deltaLocked = false;
}

//TODO use accessors once they get a boost in performance http://jsperf.com/defineproperties-vs-prototype, https://bugzilla.mozilla.org/show_bug.cgi?id=772334

/**
 *	Get the current state.
 *  @const @function
 */
Disjunction.Core.Journal.prototype.state = function()
{
	var states = this.states;
	return states[0];
}

/**
 *	Get the current delta.
 *	@const @function
 */
Disjunction.Core.Journal.prototype.delta = function()
{
	var deltas = this.deltas;
	return deltas[0];
}

/**
 *	Get the last state.
 *	@const @function
 */
Disjunction.Core.Journal.prototype.stateLast = function()
{
	var states = this.states;
	return states[1];
}

/**
 *	Get the last delta.
 *	@const @function
 */
Disjunction.Core.Journal.prototype.deltaLast = function()
{
	var deltas = this.deltas;
	return deltas[1];
}

/**
 *	Get the state at the specified index.
 *	@const @function
 *	@param {Number} index The index into the states array.
 *	@returns {*} The specified state.
 */
Disjunction.Core.Journal.prototype.stateAt = function(index)
{
	var states = this.states;
	return states[index];
}

/**
 *	Get the delta at the specified index.
 *	@const @function
 *	@param {Number} index The index into the deltas array.
 *	@returns {*} The specified delta.
 */
Disjunction.Core.Journal.prototype.deltaAt = function(index)
{
	var deltas = this.deltas;
	return deltas[index];
}

/**
 *	Set a state.
 *	@const @function
 *	@param {*} value The value to set as current.
 *	@param {Number} [index=0] The index to set to the specified value.
 */
Disjunction.Core.Journal.prototype.setState = function(value, index)
{
	console.log('setState', value);
	/*
	if (this.stateLocked)
	{
		throw "Error: Journals may only be updated once per App.update().";
	}
	else
		this.stateLocked = true;
	*/
	index = index || 0;
	var states = this.states;
	states[index] = value;
}

/**
 *	Set a delta.
 *	@const @function
 *	@param {*} value The value to set as current.
 *	@param {Number} [index=0] The index to set to the specified value.
 */
Disjunction.Core.Journal.prototype.setDelta = function(value, index)
{
	if (this.deltaLocked)
		throw "Error: Journals set to lockOnUpdate may only be updated once before locks clear (on App.update()).";
	else
		this.deltaLocked = true;

	index = index || 0;
	var deltas = this.deltas;
	deltas[index] = value;
}

/**
 *	Set all the Journal's states.
 *	@const @function
 *	@param {Number} [index=0] value The value to set all elements to.
 */
Disjunction.Core.Journal.prototype.setStates = function(value)
{
	var states = this.states;
	for (var i = 0; i < states.length; i++)
		states[i] = value;
}

/**
 *	Set all the Journal's deltas.
 *	@const @function
 *	@param {*} value The value to set all elements to.
 */
Disjunction.Core.Journal.prototype.setDeltas = function(value)
{
	var deltas = this.deltas;
	for (var i = 0; i < deltas.length; i++)
		deltas[i] = value;
}

/**
 *	Check whether the Journal has changed between current value and most recent historical value.
 *	@const @function
 */
Disjunction.Core.Journal.prototype.justChangedState = function()
{
	return this.changedState(0, 1);
}


//VIRTUAL / OVERRIDABLE
//TODO have default implementations in separate files
/**
 *	Update the Journal's non-current values, and calculate new delta from newest states, or new state from last state + current delta.
 *	@abstract @function
 */
Disjunction.Core.Journal.prototype.shift = function() {}

/**
 *	Check whether the Journal has changed between two designated indices.  Override to change the way equality checking is performed.
 *	@abstract @function
 *	@param {Number} from The index from which to shift.
 *	@param {Number} to The index to which to shift.
 */
Disjunction.Core.Journal.prototype.changedState = function(from, to){}


if (disjunction.WINDOW_CLASSES) 
	window.Journal = Disjunction.Core.Journal;