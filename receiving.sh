#!/bin/bash

# Copyright (C) 2008 Jan Stępień
#
# This file is part of Memo.
# 
# Memo is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
# 
# Memo is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with Memo.  If not, see <http://www.gnu.org/licenses/>.

source "$LIBDIR/globals.sh"

function receive_email {
	while read line
	do
		if [[ $( echo $line | grep "^ *> *.*=.*$" -c ) -eq 1  ]]
		then
			tmp=$( echo $line | sed -e "s/^\ *>\ *//" -e "s/\ *=\ */|/" \
				-e "s/\ /_/g" -e "s/|/\ /g" )
			word=$( echo $tmp | awk '{ print $1 }' | sed -e "s/_/\ /g" )
			ans=$( echo $tmp | awk '{ print $2 }' | sed -e "s/_/\ /g" )
			tmp=$( grep "^$word|" $MEMO_DBFILE \
				| sed -e "s/\ /_/g" -e "s/|/\ /g" )
			correct=$( echo $tmp | awk '{ print $2 }' | sed -e "s/_/\ /g" )
			positive=$( echo $tmp | awk '{ print $3 }' | sed -e "s/_/\ /g" )
			negative=$( echo $tmp | awk '{ print $4 }' | sed -e "s/_/\ /g" )
			newpositive="$positive"
			newnegative="$negative"
			if [[ $( echo "$correct" | grep "$ans" -c ) -eq 1 ]]
			then
				echo $ans ok
				let newpositive="$positive+1"
			else
				echo $ans nieok
				let newnegative="$negative+1"
			fi
			sed -i $MEMO_DBFILE -e "s/^\($word|[^|]*\)|$positive|$negative$/\1|$newpositive|$newnegative/"
		fi
	done
	return 0
}