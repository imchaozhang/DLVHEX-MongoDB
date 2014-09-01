
/**
 * @file MongoArrayAtom.cpp
 * @author Chao ZHANG
 * @date 08/31/2014
 *
 * @brief Defintion of the Mongo Array Atom.
 */


#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H */

#include "MongoArrayAtom.h"

#include "ConfigFile.h"

#include <dlvhex2/Registry.h>
#include <dlvhex2/Term.h>

#include "mongo/client/dbclient.h"
#include <sstream>
#include <string>
#include <iostream>
#include <vector>

#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string/replace.hpp>

using namespace std;
using namespace mongo;

namespace dlvhex {
    namespace mongo1 {

        MongoArrayAtom::MongoArrayAtom() : PluginAtom("mongo_array", 1) {

            addInputConstant();
            setOutputArity(2);
        }

        void
        MongoArrayAtom::retrieve(const Query& query, Answer& answer) throw (PluginError) {
            Registry &registry = *getRegistry();
            
            const Term& in_array = registry.terms.getByID(query.input[0]);
            if (!in_array.isQuotedString()) {
                    throw PluginError("Wrong input argument type for input 1, need to be quoted string");
                }
                
            std::string s_array = in_array.getUnquotedString();                
            if(s_array.substr(0,1) != "[" && s_array.substr(s_array.size()-1,1) != "]") {
            
            	throw PluginError("The array input should be embedd in the \"[ ]\" ");
            
            }  
            
            
            BSONObj b_array = fromjson("{array:" + s_array + "}");
            
            if(b_array["array"].Obj().couldBeArray()){
		
            	vector<BSONElement> elements = b_array["array"].Array();
                for (int i = 0; i < elements.size(); i++) {
                
                    Tuple out;
                    
                    std::string s = elements[i].toString(false);
                    boost::replace_all(s,"\"","'");
                    if(s.substr(0,1)=="'" && s.substr(s.size()-1,1) == "'"){
                       boost::replace_all(s,"'",""); 
                    }//end if
                    
                   if(s.size()>=3 && s.substr(s.size()-2,2) == ".0") {
                   	s = s.substr(0, s.size()-2);
                   }//end if

			try {
                                int v;
                                v = boost::lexical_cast<int>(s);
                                
			   	Term index_term(ID::MAINKIND_TERM | ID::SUBKIND_TERM_CONSTANT,boost::lexical_cast<string>(i));
		           	Term elem_term(ID::MAINKIND_TERM | ID::SUBKIND_TERM_CONSTANT, s);
		           	out.push_back(registry.storeTerm(index_term));
		           	out.push_back(registry.storeTerm(elem_term));

                            } catch (boost::bad_lexical_cast& e) {

			   	Term index_term(ID::MAINKIND_TERM | ID::SUBKIND_TERM_CONSTANT,boost::lexical_cast<string>(i));
		           	Term elem_term(ID::MAINKIND_TERM | ID::SUBKIND_TERM_CONSTANT, '"' + s + '"');
		           	out.push_back(registry.storeTerm(index_term));
		           	out.push_back(registry.storeTerm(elem_term));
                            }

                   

		   answer.get().push_back(out);

                }//end for

            }
            
            else {
            
            	throw PluginError("The input is not an Array!");
            
            }
            
   	
        } // end of retrieve function

    } // end of mongo
} // end of dlvhex
