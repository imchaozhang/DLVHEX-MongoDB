
/**
 * @file MongoJSONAtom.cpp
 * @author Chao ZHANG
 * @date 09/01/2014
 *
 * @brief Defintion of the Mongo JSON Atom.
 */


#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H */

#include "MongoJSONAtom.h"

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

        MongoJSONAtom::MongoJSONAtom() : PluginAtom("mongo_json", 1) {

            addInputConstant();
            setOutputArity(2);
        }

        void
        MongoJSONAtom::retrieve(const Query& query, Answer& answer) throw (PluginError) {
            Registry &registry = *getRegistry();
            
            const Term& in_json = registry.terms.getByID(query.input[0]);
            if (!in_json.isQuotedString()) {
                    throw PluginError("Wrong input argument type for input 1, need to be quoted string");
                }
                
            std::string s_json = in_json.getUnquotedString();                
            if(s_json.substr(0,1) != "{" && s_json.substr(s_json.size()-1,1) != "}") {
            
            	throw PluginError("The json input should be embedd in the \"{ }\" ");
            
            }  
            
            BSONObj b_json;
            
            try {
 	           b_json = fromjson(s_json);
            
            } catch(const mongo::DBException &e) {
            	
            	ostringstream out_catch;
            	out_catch << e.what();
            	throw PluginError("catch: " + out_catch.str());
            
            }
            
       	    for( BSONObj::iterator i = b_json.begin(); i.more(); ) { 
                    Tuple out;
                           	    	    
       	    	    BSONElement e = i.next();
                    
                    std::string s_key = e.fieldName();
                    std::string s_value;       	    	    
       	    	    
       	    	    
       	    	    if(e.type() == 2){ // type string
       	    	    
       	    	    	s_value = e.String();
       	    	    
       	    	    }
       	    	    
       	    	    else if(e.isNumber()) { // type number
       	    	    
       	    	        ostringstream convert;
                        convert << e.toString(false);
                        s_value = convert.str();
                        // if it is int, remove the last two digits
                        if(s_value.size()>=3 && s_value.substr(s_value.size()-2,2) == ".0")
                        s_value = s_value.substr(0, s_value.size()-2);
       	    	    
       	    	    }
       	    	    
       	    	    else if(e.type() == 9) { // type date
       	    	    
       	    	        ostringstream convert;         
        		convert << e.date();
        		s_value = convert.str();
        		if(s_value.size() == 13)
        		// remove millionsecond to the epoch time stamp
        		s_value = s_value.substr(0,10);
    
       	    	    
       	    	    }
       	    	    
       	    	    else if (e.isBoolean()) {
       	    	    
       	    	    	  ostringstream convert;
       	    	    	  
       	    	    	  if (e.boolean() == true)
                              s_value = "true";
                          else
                              s_value = "false";

                    }
                    
                    else if(!e.eoo()){

       	    	    	  s_value = e.toString(false);
       	    	    	  
       	    	    	  boost::replace_all(s_value,"\"","'");
       	    	    	         				
                    	  if(s_value.substr(0,1)=="'" && s_value.substr(s_value.size()-1,1) == "'"){
                       		
                       		boost::replace_all(s_value,"'",""); 
                       		
                    	  }         				    		
       	    	    
       	    	    }
       	    	    
       	    	    else {
       	    	    
       	    	    	s_value = "";
       	    	    
       	    	    }
       	    	    

			try {
                                int v;
                                v = boost::lexical_cast<int>(s_value);
                                
			   	Term key_term(ID::MAINKIND_TERM | ID::SUBKIND_TERM_CONSTANT,'"' + s_key + '"');
		           	Term value_term(ID::MAINKIND_TERM | ID::SUBKIND_TERM_CONSTANT, s_value);
		           	out.push_back(registry.storeTerm(key_term));
		           	out.push_back(registry.storeTerm(value_term));

                            } catch (boost::bad_lexical_cast& e) {

			   	Term key_term(ID::MAINKIND_TERM | ID::SUBKIND_TERM_CONSTANT,'"' + s_key + '"');
		           	Term value_term(ID::MAINKIND_TERM | ID::SUBKIND_TERM_CONSTANT, '"' +  s_value + '"');
		           	out.push_back(registry.storeTerm(key_term));
		           	out.push_back(registry.storeTerm(value_term));
                            }

                   

		   answer.get().push_back(out);

                }//end for

        } // end of retrieve function

    } // end of mongo
} // end of dlvhex
