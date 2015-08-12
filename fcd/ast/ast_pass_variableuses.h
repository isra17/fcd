//
// ast_pass_variableuses.h
// Copyright (C) 2015 Félix Cloutier.
// All Rights Reserved.
//
// This file is part of fcd.
//
// fcd is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// fcd is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with fcd.  If not, see <http://www.gnu.org/licenses/>.
//

#ifndef ast_pass_variableuses_cpp
#define ast_pass_variableuses_cpp

#include "ast_nodes.h"
#include "ast_pass.h"

#include <deque>
#include <limits>
#include <list>
#include <map>
#include <unordered_map>

struct VariableUse
{
	NOT_NULL(Statement) owner;
	NOT_NULL(Expression*) location;
	size_t index;
	
	inline VariableUse(Statement* owner, Expression** location, size_t index)
	: owner(owner), location(location), index(index)
	{
	}
};

struct VariableUses
{
	typedef std::list<VariableUse>::iterator iterator;
	
	Expression* expression;
	std::list<VariableUse> defs;
	std::list<VariableUse> uses;
	
	VariableUses(Expression* expr);
	
	bool usedBeforeDefined() const;
};

class AstVariableUses : public AstPass
{
	size_t index;
	std::deque<Expression*> declarationOrder;
	std::unordered_map<Expression*, VariableUses> declarationUses;
	std::map<size_t, size_t> loopRanges;
	
	void visit(Statement* owner, Expression** expression, bool isDef = false);
	void visit(Statement* statement);
	
protected:
	virtual void doRun(FunctionNode& fn) override;
	
public:
	static constexpr size_t MaxIndex = std::numeric_limits<size_t>::max();
	typedef decltype(declarationOrder)::const_iterator iterator;
	
	virtual const char* getName() const override;
	
	iterator begin() const { return declarationOrder.begin(); }
	iterator end() const { return declarationOrder.end(); }
	
	VariableUses& getUseInfo(iterator iter);
	VariableUses* getUseInfo(Expression* expr);
	
	void replaceUseWith(VariableUses& use, VariableUses::iterator iter, Expression* replacement);
	
	size_t innermostLoopIndexOfUse(const VariableUse& use) const;
	std::pair<VariableUses::iterator, VariableUses::iterator> usesReachedByDef(VariableUses& uses, VariableUses::iterator def) const;
	std::pair<VariableUses::iterator, VariableUses::iterator> defsReachingUse(VariableUses& uses, VariableUses::iterator use) const;
	
	void dump() const;
};

#endif /* ast_pass_variableuses_cpp */