#include "HFS_ScriptCompiler.hpp"
#include <algorithm>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <regex>
#include "./utility/HFS_Utility.hpp"
#include "core/HFS_TokenPattern.hpp"
#include "operations/HFS_Operations.hpp"

namespace fs = std::filesystem;

namespace hfs {
    bool ScriptCompiler::is_token_separator(const char& c) {
        return c == ' '  ||
               c == '\t' ||
               c == '\f' ||
               c == '\v' ||
               c == '\n';
    }

    ScriptCompiler::ScriptCompiler(bool default_patterns) {
        if(default_patterns) {// TODO: overview deletes etc
            // TODO: way to make continue/break work
            // TODO: break/continue needs to know how many scopes to delete
            /* IDEA
            map<OpType> aux_ops {
                OpType::Loop
                OpType::Next
                OpType::GOTO?? :thinking:
            }

            */
            //aux patterns
            auto tp_param_comma = new core::TokenPattern(core::PatternType::Definition);// name,
            auto tp_param_list = new core::TokenPattern(core::PatternType::Definition);// tp_param_comma name
            auto tp_value_comma = new core::TokenPattern(core::PatternType::Definition);// value,
            auto tp_value_list = new core::TokenPattern(core::PatternType::Definition);// tp_value_comma value
            auto tp_else = new core::TokenPattern(core::PatternType::Definition);//else { ... }
            auto tp_elif = new core::TokenPattern(core::PatternType::Definition);//else if (value) { ... }

            //common patterns
            auto tp_sub_scope = new core::TokenPattern(core::PatternType::Definition);// { ... }
            auto tp_code_line = new core::TokenPattern(core::PatternType::Definition);// value ;
            auto tp_while_loop = new core::TokenPattern(core::PatternType::Definition);// while(value) { ... }
            auto tp_if_conditional = new core::TokenPattern(core::PatternType::Definition);// if(value) { ... } else if(value) { ... } else { ... }
            
            auto tp_return_value = new core::TokenPattern(core::PatternType::Void);// return value?;
            auto tp_release = new core::TokenPattern(core::PatternType::Void);// release;

            auto tp_sqr_bracket_val = new core::TokenPattern(core::PatternType::Definition);// [ list ]
            auto tp_var_retrieve = new core::TokenPattern(core::PatternType::Value);// name tp_sqr_bracket_val
            auto tp_func_def = new core::TokenPattern(core::PatternType::Definition);// func name (tp_param_list) tp_sub_scope
            auto tp_var_set = new core::TokenPattern(core::PatternType::Value);// var_retrieve = value
            auto tp_raw = new core::TokenPattern(core::PatternType::Value);// raw
            auto tp_func_call = new core::TokenPattern(core::PatternType::Value);// name ( value, value )
            auto tp_paren_value = new core::TokenPattern(core::PatternType::Value);// ( value )
            auto tp_single_op_value = new core::TokenPattern(core::PatternType::Value);// !value
            auto tp_double_op_value = new core::TokenPattern(core::PatternType::Value);// value + value
            auto tp_ternary_op_value = new core::TokenPattern(core::PatternType::Value);// bool ? value1 : value2


            std::vector<core::TokenPattern*> midcode_patterns = { tp_sub_scope, tp_code_line, tp_while_loop, tp_if_conditional,
                                                                  tp_return_value, tp_release };
            std::vector<core::TokenPattern*> value_patterns = { tp_var_retrieve, tp_var_set, tp_raw, tp_func_call, tp_paren_value,
                                                                tp_double_op_value, tp_single_op_value, tp_ternary_op_value };

            tp_param_comma->add_key_data(core::PatternData(new core::TokenTypePatternKey(core::TokenType::Name)));
            tp_param_comma->add_key_data(core::PatternData(new core::TextPatternKey(",")));
            
            tp_param_list->add_key_data(core::PatternData(new core::OtherPatternKey(tp_param_comma), 0, -1));
            tp_param_list->add_key_data(core::PatternData(new core::TokenTypePatternKey(core::TokenType::Name)));

            tp_value_comma->add_key_data(core::PatternData(new core::PatternListPatternKey(core::PatternType::Value, value_patterns)));//0
            tp_value_comma->add_key_data(core::PatternData(new core::TextPatternKey(",")));                                            //1

            tp_value_list->add_key_data(core::PatternData(new core::OtherPatternKey(tp_value_comma), 0, -1));                         //0
            tp_value_list->add_key_data(core::PatternData(new core::PatternListPatternKey(core::PatternType::Value, value_patterns)));//1

            tp_else->add_key_data(core::PatternData(new core::TextPatternKey("else")));       //0
            tp_else->add_key_data(core::PatternData(new core::OtherPatternKey(tp_sub_scope)));//1

            tp_elif->add_key_data(core::PatternData(new core::TextPatternKey("else")));                                         //0
            tp_elif->add_key_data(core::PatternData(new core::TextPatternKey("if")));                                           //1
            tp_elif->add_key_data(core::PatternData(new core::TextPatternKey("(")));                                            //2
            tp_elif->add_key_data(core::PatternData(new core::PatternListPatternKey(core::PatternType::Value, value_patterns)));//3
            tp_elif->add_key_data(core::PatternData(new core::TextPatternKey(")")));                                            //4
            tp_elif->add_key_data(core::PatternData(new core::OtherPatternKey(tp_sub_scope)));                                  //5

            tp_sub_scope->add_key_data(core::PatternData(new core::TextPatternKey("{")));                                                   //0
            tp_sub_scope->add_key_data(core::PatternData(new core::PatternListPatternKey(core::PatternType::Any, midcode_patterns), 0, -1));//1
            tp_sub_scope->add_key_data(core::PatternData(new core::TextPatternKey("}")));                                                   //2
            tp_sub_scope->set_compile_function([] (core::MatchResult* res, core::CompilationData& c_data) -> core::CompilationResult {
                auto fs = new FlattenScopeOperation();// }
                fs->set_next_operation(c_data.aux_operations.get(core::AuxOpType::Next).op);
                c_data.depth++;

                auto vec = std::vector<Operation*>();
                vec.push_back(fs);// }
                auto matches = res->groups[1].matches;//lines & etc
                std::reverse(matches.begin(), matches.end());
                for(auto m : matches) {
                    c_data.aux_operations.push(core::AuxOpType::Next, core::AuxOp(vec.back(), c_data.depth));
                    auto other_ops = m.sub_match->pattern->compile(m.sub_match, c_data).operations;
                    for(auto b_itr = other_ops.rbegin(); b_itr != other_ops.rend(); ++b_itr) {
                        vec.push_back(*b_itr);
                    }
                }
                c_data.depth--;
                auto ds = new DeepenScopeOperation();
                ds->set_next_operation(vec.back());
                vec.push_back(ds);// { 
                c_data.aux_operations.push(core::AuxOpType::Next, core::AuxOp(vec.back(), c_data.depth));          

                std::reverse(vec.begin(), vec.end());//reverse back to normal order
                return core::CompilationResult(vec);
            });

            tp_code_line->add_key_data(core::PatternData(new core::PatternListPatternKey(core::PatternType::Any, value_patterns)));//0
            tp_code_line->add_key_data(core::PatternData(new core::TextPatternKey(";")));                                            //1
            tp_code_line->set_compile_function([] (core::MatchResult* res, core::CompilationData& c_data) -> std::vector<Operation*> {
                auto match = res->groups[0].matches[0].sub_match;//only uses main match
                auto op = match->pattern->compile(match, c_data).operations[0];
                auto seq = dynamic_cast<SequentialOperation*>(op);
                if(!seq) {
                    auto sub = new SubCallOperation(op);
                    op = seq = sub;
                }
                seq->set_next_operation(c_data.aux_operations.get(core::AuxOpType::Next).op);
                return std::vector<Operation*> { op };
            });

            tp_while_loop->add_key_data(core::PatternData(new core::TextPatternKey("while")));                                        //0
            tp_while_loop->add_key_data(core::PatternData(new core::TextPatternKey("(")));                                            //1
            tp_while_loop->add_key_data(core::PatternData(new core::PatternListPatternKey(core::PatternType::Value, value_patterns)));//2
            tp_while_loop->add_key_data(core::PatternData(new core::TextPatternKey(")")));                                            //3
            tp_while_loop->add_key_data(core::PatternData(new core::OtherPatternKey(tp_sub_scope)));                                  //4
            tp_while_loop->set_compile_function([] (core::MatchResult* res, core::CompilationData& c_data) -> core::CompilationResult {
                auto vec = std::vector<Operation*>();
                
                auto cond_match = res->groups[2].matches[0].sub_match;
                auto branch = new BranchOperation(cond_match->pattern->compile(cond_match, core::CompilationData()).operations[0]);
                branch->set_false_operation(c_data.aux_operations.get(core::AuxOpType::Next, false).op);

                c_data.aux_operations.push(core::AuxOpType::LoopEntry, core::AuxOp(branch, c_data.depth));
                c_data.aux_operations.push(core::AuxOpType::LoopExit, c_data.aux_operations.get(core::AuxOpType::Next));
                
                auto scope_match = res->groups[4].matches[0].sub_match;
                auto scope_res = scope_match->pattern->compile(scope_match, c_data);

                branch->set_true_operation(scope_res.operations[0]);
                
                vec.push_back(branch);
                for(auto op : scope_res.operations) {
                    vec.push_back(op);
                }
                auto seq = dynamic_cast<SequentialOperation*>(vec.back());
                seq->set_next_operation(branch);
                
                return core::CompilationResult(vec);
            });

            tp_if_conditional->add_key_data(core::PatternData(new core::TextPatternKey("if")));                                           //0
            tp_if_conditional->add_key_data(core::PatternData(new core::TextPatternKey("(")));                                            //1
            tp_if_conditional->add_key_data(core::PatternData(new core::PatternListPatternKey(core::PatternType::Value, value_patterns)));//2
            tp_if_conditional->add_key_data(core::PatternData(new core::TextPatternKey(")")));                                            //3
            tp_if_conditional->add_key_data(core::PatternData(new core::OtherPatternKey(tp_sub_scope)));                                  //4
            tp_if_conditional->add_key_data(core::PatternData(new core::OtherPatternKey(tp_elif), 0, -1));                                //5
            tp_if_conditional->add_key_data(core::PatternData(new core::OtherPatternKey(tp_else), 0, 1));                                 //6
            tp_if_conditional->set_compile_function([] (core::MatchResult* res, core::CompilationData& c_data) -> core::CompilationResult {
                auto exit_op = c_data.aux_operations.get(core::AuxOpType::Next).op;
                auto vec = std::vector<Operation*>();
                
                //does everything backwards bc we need to know next operations and such
                auto else_matches = res->groups[6].matches;
                if(else_matches.size() > 0) {//TODO: check if next op is working ok here bc of c_data
                    auto m = else_matches[0].sub_match->groups[1].matches[0].sub_match;
                    auto ops = m->pattern->compile(m, c_data).operations;
                    for(auto op : ops) {
                        vec.push_back(op);
                    }
                }

                auto elif_matches = res->groups[5].matches;
                for(auto itr = elif_matches.rbegin(); itr != elif_matches.rend(); ++itr) {
                    auto vm = itr->sub_match->groups[3].matches[0].sub_match;//value match
                    auto branch = new BranchOperation(vm->pattern->compile(vm, core::CompilationData()).operations[0]);
                    branch->set_false_operation(c_data.aux_operations.get(core::AuxOpType::Next).op);

                    auto sm = itr->sub_match->groups[5].matches[0].sub_match;//scope match
                    auto ops = sm->pattern->compile(sm, c_data).operations;

                    branch->set_true_operation(ops[0]);

                    auto seq = dynamic_cast<SequentialOperation*>(ops.back());//if true, skips other elifs/elses
                    seq->set_next_operation(exit_op);

                    c_data.aux_operations.push(core::AuxOpType::Next, core::AuxOp(branch, c_data.depth));
                    for(auto r_itr = ops.rbegin(); r_itr != ops.rend(); ++r_itr) {
                        vec.insert(vec.begin(), *r_itr);
                    }
                    vec.insert(vec.begin(), branch);
                }

                {//main match
                    auto vm = res->groups[2].matches[0].sub_match;
                    auto branch = new BranchOperation(vm->pattern->compile(vm, core::CompilationData()).operations[0]);
                    branch->set_false_operation(c_data.aux_operations.get(core::AuxOpType::Next).op);

                    auto sm = res->groups[4].matches[0].sub_match;
                    auto ops = sm->pattern->compile(sm, c_data).operations;

                    branch->set_true_operation(ops[0]);

                    auto seq = dynamic_cast<SequentialOperation*>(ops.back());//if true, skips other elifs/elses
                    seq->set_next_operation(exit_op);

                    c_data.aux_operations.push(core::AuxOpType::Next, core::AuxOp(branch, c_data.depth));
                    for(auto r_itr = ops.rbegin(); r_itr != ops.rend(); ++r_itr) {
                        vec.insert(vec.begin(), *r_itr);
                    }
                    vec.insert(vec.begin(), branch);
                }

                return core::CompilationResult(vec);
            });

            tp_return_value->add_key_data(core::PatternData(new core::TextPatternKey("return")));                                        //0
            tp_return_value->add_key_data(core::PatternData(new core::PatternListPatternKey(core::PatternType::Value, value_patterns), 0, 1)); //1
            tp_return_value->add_key_data(core::PatternData(new core::TextPatternKey(";")));                                        //2
            tp_return_value->set_compile_function([] (core::MatchResult* res, core::CompilationData& c_data) -> core::CompilationResult {
                Operation* value_op = nullptr;
                if(res->groups[1].matches.size() > 0) {
                    auto value_match = res->groups[1].matches[0].sub_match;
                    value_op = value_match->pattern->compile(value_match, core::CompilationData()).operations[0];
                }
                else {
                    value_op = new RawValueOperation("null");
                }

                return core::CompilationResult({ new ReturnOperation(value_op) });
            });

            tp_release->add_key_data(core::PatternData(new core::TextPatternKey("release"))); //0
            tp_release->add_key_data(core::PatternData(new core::TextPatternKey(";")));      //1
            tp_release->set_compile_function([] (core::MatchResult* res, core::CompilationData& c_data) -> core::CompilationResult {
                return core::CompilationResult({ new ReleaseOperation() });
            });


            tp_sqr_bracket_val->add_key_data(core::PatternData(new core::TextPatternKey("[")));
            tp_sqr_bracket_val->add_key_data(core::PatternData(new core::PatternListPatternKey(core::PatternType::Value, value_patterns)));
            tp_sqr_bracket_val->add_key_data(core::PatternData(new core::TextPatternKey("]")));
            tp_sqr_bracket_val->set_compile_function([] (core::MatchResult* res, core::CompilationData& c_data) -> core::CompilationResult {
                auto match = res->groups[1].matches[0].sub_match;
                return core::CompilationResult({ match->pattern->compile(match, core::CompilationData()) });
            });

            tp_var_retrieve->add_key_data(core::PatternData(new core::TokenTypePatternKey(core::TokenType::Name)));
            tp_var_retrieve->add_key_data(core::PatternData(new core::OtherPatternKey(tp_sqr_bracket_val), 0, -1));
            tp_var_retrieve->set_compile_function([] (core::MatchResult* res, core::CompilationData& c_data) -> core::CompilationResult {
                auto op = new VariableRetrievalOperation(res->groups[0].matches[0].matched_token.token);
                auto key_matches = res->groups[1].matches;
                for(auto k : key_matches) {
                    op->add_dictionary_key(k.sub_match->pattern->compile(k.sub_match, core::CompilationData()).operations[0]);
                }

                return core::CompilationResult({ op });
            });

            tp_var_set->add_key_data(core::PatternData(new core::OtherPatternKey(tp_var_retrieve)));
            tp_var_set->add_key_data(core::PatternData(new core::TextPatternKey("=")));
            tp_var_set->add_key_data(core::PatternData(new core::PatternListPatternKey(core::PatternType::Value, value_patterns)));
            tp_var_set->set_compile_function([] (core::MatchResult* res, core::CompilationData& c_data) -> core::CompilationResult {
                auto retrieve_mr = res->groups[0].matches[0].sub_match;
                auto retrieve_op = dynamic_cast<VariableRetrievalOperation*>(retrieve_mr->pattern->compile(retrieve_mr, core::CompilationData()).operations[0]);

                auto value_mr = res->groups[2].matches[0].sub_match;
                auto value_op = value_mr->pattern->compile(value_mr, core::CompilationData()).operations[0];

                auto op = new SetOperation(retrieve_op, value_op);
                return core::CompilationResult({ op });
            });

            tp_raw->add_key_data(core::PatternData(new core::TokenTypePatternKey(core::TokenType::RawValue)));
            tp_raw->set_compile_function([] (core::MatchResult* res, core::CompilationData& c_data) -> core::CompilationResult {
                return core::CompilationResult({ new RawValueOperation(res->groups[0].matches[0].matched_token.token) });
            });

            tp_func_call->add_key_data(core::PatternData(new core::TokenTypePatternKey(core::TokenType::Name)));//0
            tp_func_call->add_key_data(core::PatternData(new core::TextPatternKey("(")));                       //1
            tp_func_call->add_key_data(core::PatternData(new core::OtherPatternKey(tp_value_list), 0, 1));      //2
            tp_func_call->add_key_data(core::PatternData(new core::TextPatternKey(")")));                       //3
            tp_func_call->set_compile_function([] (core::MatchResult* res, core::CompilationData& c_data) -> core::CompilationResult {
                auto func_name = res->groups[0].matches[0].matched_token.token;
                std::vector<Operation*> params;
                
                auto list_matches = res->groups[2].matches;
                if(list_matches.size() > 0) {
                    auto match = list_matches[0];//tp_value_list
                    auto comma_params = match.sub_match->groups[0].matches;
                    for (auto comma_param : comma_params)
                    {
                        auto p = comma_param.sub_match->groups[0].matches[0].sub_match;//param data match
                        params.push_back(p->pattern->compile(p, core::CompilationData()).operations[0]);
                    }
                    auto last_param = match.sub_match->groups[1].matches[0].sub_match;
                    params.push_back(last_param->pattern->compile(last_param, core::CompilationData()).operations[0]);
                }

                return core::CompilationResult({ new SubCallOperation(new FunctionCallOperation(func_name, params)) });
            });

            tp_paren_value->add_key_data(core::PatternData(new core::TextPatternKey("(")));                                            //0
            tp_paren_value->add_key_data(core::PatternData(new core::PatternListPatternKey(core::PatternType::Value, value_patterns)));//1
            tp_paren_value->add_key_data(core::PatternData(new core::TextPatternKey(")")));                                            //2
            tp_paren_value->set_compile_function([] (core::MatchResult* res, core::CompilationData& c_data) -> core::CompilationResult {
                auto mr = res->groups[1].matches[0].sub_match;
                return mr->pattern->compile(mr, core::CompilationData());//TODO: see if this works properly for ordering functions
            });

            tp_single_op_value->add_key_data(core::PatternData(new core::TokenTypePatternKey(core::TokenType::Operator)));                 //0
            tp_single_op_value->add_key_data(core::PatternData(new core::PatternListPatternKey(core::PatternType::Value, value_patterns)));//1
            tp_single_op_value->set_compile_function([] (core::MatchResult* res, core::CompilationData& c_data) -> core::CompilationResult {
                auto operator_str = res->groups[0].matches[0].matched_token.token;
                auto value_match = res->groups[1].matches[0].sub_match;
                auto value_op = value_match->pattern->compile(value_match, core::CompilationData()).operations[0];
                
                return core::CompilationResult({ new OperatorOperation(operator_str, value_op) });
            });

            tp_double_op_value->add_key_data(core::PatternData(new core::PatternListPatternKey(core::PatternType::Value, value_patterns))); //0
            tp_double_op_value->add_key_data(core::PatternData(new core::TokenTypePatternKey(core::TokenType::Operator)));                  //1
            tp_double_op_value->add_key_data(core::PatternData(new core::PatternListPatternKey(core::PatternType::Value, value_patterns))); //2
            tp_double_op_value->set_compile_function([] (core::MatchResult* res, core::CompilationData& c_data) -> core::CompilationResult {
                auto operator_str = res->groups[1].matches[0].matched_token.token;

                auto value1_match = res->groups[0].matches[0].sub_match;
                auto value2_match = res->groups[2].matches[0].sub_match;

                auto value1_op = value1_match->pattern->compile(value1_match, core::CompilationData()).operations[0];
                auto value2_op = value2_match->pattern->compile(value2_match, core::CompilationData()).operations[0];
                
                return core::CompilationResult({ new OperatorOperation(operator_str, value1_op, value2_op) });
            });

            tp_ternary_op_value->add_key_data(core::PatternData(new core::PatternListPatternKey(core::PatternType::Value, value_patterns))); //0
            tp_ternary_op_value->add_key_data(core::PatternData(new core::TextPatternKey("?")));                                             //1
            tp_ternary_op_value->add_key_data(core::PatternData(new core::PatternListPatternKey(core::PatternType::Value, value_patterns))); //2
            tp_ternary_op_value->add_key_data(core::PatternData(new core::TextPatternKey(":")));                                             //3
            tp_ternary_op_value->add_key_data(core::PatternData(new core::PatternListPatternKey(core::PatternType::Value, value_patterns))); //4
            tp_ternary_op_value->set_compile_function([] (core::MatchResult* res, core::CompilationData& c_data) -> core::CompilationResult {
                auto cond_match = res->groups[0].matches[0].sub_match;
                auto v1_match = res->groups[2].matches[0].sub_match;
                auto v2_match = res->groups[4].matches[0].sub_match;

                auto cond_op = cond_match->pattern->compile(cond_match, core::CompilationData()).operations[0];
                auto v1_op = new ReturnOperation(v1_match->pattern->compile(v1_match, core::CompilationData()).operations[0]);
                auto v2_op = new ReturnOperation(v2_match->pattern->compile(v2_match, core::CompilationData()).operations[0]);

                auto branch = new BranchOperation(cond_op);
                branch->set_true_operation(v1_op);
                branch->set_false_operation(v2_op);

                return core::CompilationResult({ branch });
            });

            tp_func_def->add_key_data(core::PatternData(new core::TextPatternKey("func")));                    //0
            tp_func_def->add_key_data(core::PatternData(new core::TokenTypePatternKey(core::TokenType::Name)));//1
            tp_func_def->add_key_data(core::PatternData(new core::TextPatternKey("(")));                       //2
            tp_func_def->add_key_data(core::PatternData(new core::OtherPatternKey(tp_param_list), 0, 1));      //3
            tp_func_def->add_key_data(core::PatternData(new core::TextPatternKey(")")));                       //4
            tp_func_def->add_key_data(core::PatternData(new core::OtherPatternKey(tp_sub_scope)));             //5
            tp_func_def->set_compile_function([] (core::MatchResult* res, core::CompilationData& c_data) -> core::CompilationResult {
                auto sub_match = res->groups[5].matches[0].sub_match;//sub scope match
                
                auto ops = sub_match->pattern->compile(sub_match, c_data).operations;

                auto func_name = res->groups[1].matches[0].matched_token.token;

                auto params = std::vector<std::string>();
                auto param_matches = res->groups[3].matches;
                if(param_matches.size() > 0) {
                    auto p_comma_list = param_matches[0].sub_match->groups[0].matches;
                    for(auto m : p_comma_list) {
                        auto p = m.sub_match->groups[0].matches[0].matched_token.token;
                        params.push_back(p);
                    }

                    auto last_param = param_matches[0].sub_match->groups[1].matches[0].matched_token.token;
                    params.push_back(last_param);
                }
                
                return core::CompilationResult(ops, { ScriptFunctionDef(func_name, params) });
            });

            //main pattern
            main_pattern = new core::TokenPattern(core::PatternType::Definition);                                    //0
            main_pattern->add_key_data(core::PatternData(new core::OtherPatternKey(tp_func_def), 0, -1));            //1
            main_pattern->add_key_data(core::PatternData(new core::TokenTypePatternKey(core::TokenType::EndOfFile)));//2
            main_pattern->set_compile_function([] (core::MatchResult* res, core::CompilationData& c_data) -> core::CompilationResult {
                auto ops = std::vector<Operation*>();
                auto func_defs = std::vector<ScriptFunctionDef>();
                
                auto func_matches = res->groups[1].matches;
                for(auto m : func_matches) {
                    c_data.aux_operations.reset();
                    auto cr = m.sub_match->pattern->compile(m.sub_match, c_data);

                    for(auto op : cr.operations) {
                        ops.push_back(op);
                    }
                    for(auto fd : cr.function_defs) {
                        func_defs.push_back(fd);
                    }
                }

                return core::CompilationResult(ops, func_defs);
            });
        }
    }

    void ScriptCompiler::push_log(const LogType type, const std::string text, core::Token const& token) {
        std::string msg = std::regex_replace(text, std::regex("%t"), token.token);
        std::stringstream ss;
        ss << msg << " (Ln " << token.line << ", Col " << token.column << ')';
        LogData new_log = { type, ss.str() };
        logs.push_back(new_log);
    }

    std::vector<core::TokenMatcher> ScriptCompiler::tokenize_from_file(std::string path) {
        fs::path p(path);
        if(fs::exists(p) && fs::is_regular_file(p)) {
            std::string file_text;
            std::ifstream file(p);

            char c;
            while(file.get(c)) {
                file_text += c;
            }
            return tokenize_from_text(file_text);
        }

        return std::vector<core::TokenMatcher>();
    }

    std::vector<core::TokenMatcher> ScriptCompiler::tokenize_from_text(std::string text) {
        std::remove_if(text.begin(), text.end(), [] (char const& ch) { return ch == '\r'; });

        auto itr = text.begin();
        unsigned int line = 1;
        unsigned int column = 0;
        auto peek_itr= [&] (char* c) -> bool {
            if(itr != text.end() && (itr + 1) != text.end()) {
                if(c != nullptr) {
                    *c = *(itr + 1); 
                }
                return true;
            }
            return false;
        };

        auto step_pos = [&] (const char c) {
            ++column;
            switch (c)
            {
            case '\n':
                ++line;
                column = 0;
                break;
            case '\t':
                column += 4 - (column % 4);//tabs account for 4 spaces
                break;
            }
            ///   aaa
        };

        auto step_itr= [&] (char* c) -> bool {
            if(itr != text.end() && ++itr != text.end()) {
                step_pos(*itr);

                if(c != nullptr) {
                    *c = *itr; 
                }
                return true;
            }
            return false;
        };

        auto token_vector = std::vector<core::TokenMatcher>();
        if(itr == text.end()) {//premature return
            return token_vector;
        }

        core::Token current_token;
        auto reset_token = [&current_token, &line, &column] () {
            current_token = { core::TokenType::Invalid, "", line, column };
        };
        reset_token();

        auto push_token = [&reset_token, &current_token, &token_vector] () {
            if(current_token.token.size() > 0) {
                current_token.type = utility::determine_token_type(current_token.token);
                token_vector.push_back(current_token);
            }
            reset_token();
        };

        auto char_to_token = [&current_token, &reset_token] (const char c) {
            if(current_token.token.size() == 0) {
                reset_token();
            }
            current_token.token += c;
        };

        bool is_code = true;

        char c = *itr;//char to check
        step_pos(c);
        do {
            if(is_code) {
                if(c == '#') {//found comment, will skip until \n
                    push_token();
                    while (step_itr(&c) && c != '\n'){}
                    continue;
                }
                else if (c == '\"') {//start text
                    is_code = false;
                    push_token();
                    char_to_token(c);
                    continue;
                }
                else if (utility::is_operator_symbol(c)) {
                    push_token();
                    std::string operator_str = "";
                    operator_str += c;
                    char new_c;
                    while(peek_itr(&new_c) && utility::is_operator_symbol(new_c)) {
                        step_itr(&c);
                        operator_str += c;
                    }
                    //if next token is likely a number, consumes last operator if it is a minus sign
                    bool minus_cut = (new_c == 46 || (new_c >= 48 && new_c <=57)) && operator_str[operator_str.length() - 1] == '-';
                    for(int i = 0; i < operator_str.length() - (minus_cut ? 1 : 0); ++i) {
                        char_to_token(operator_str[i]);
                    }
                    push_token();
                    if(minus_cut) {
                        char_to_token('-');
                    }
                }
                else if (utility::is_special_symbol(c)) {
                    push_token();
                    char_to_token(c);//pushes the symbol as its own token
                    push_token();
                }
                else if (is_token_separator(c)) {
                    push_token();
                }
                else { 
                    char_to_token(c);
                }
            }
            else {//text, find text-ending conditions, or just copy text to token
                if(c == '\"') {//end text
                    char_to_token(c);
                    push_token();
                    is_code = true;
                }
                else if (c == '\\') {//escaped char
                    if(step_itr(&c)) {
                        if(c == 'n') {
                            char_to_token('\n');
                        }
                        else {
                            char_to_token(c);
                        }
                    }
                }
                else {
                    char_to_token(c);
                }
            }
        } while(step_itr(&c));
        token_vector.push_back(core::Token(core::TokenType::EndOfFile, "EOF", line, column));

        if(!is_code) {
            push_log(LogType::Error, "Script ended unexpectedly!", current_token);
        }
        if(token_vector.size() == 0) {
            push_log(LogType::Error, "No tokens found!", current_token);
        }

        // TODO: remove pattern testing stuff
        auto token_itr = token_vector.begin();
        core::Token bad_token = core::Token(core::TokenType::Invalid);
        auto mr = main_pattern->try_match(token_itr, &bad_token);
        auto ops = std::vector<Operation*>();
        if(mr != nullptr) {
            auto matches = mr->groups[0].matches;
            for(auto& m : matches) {
                auto result = m.sub_match->pattern->compile(m.sub_match, core::CompilationData(0, core::AuxOpVector()));
                auto new_ops = result.operations;
                for(auto& n_op : new_ops) {
                    ops.push_back(n_op);
                }
            }
        }
        else {
            push_log(LogType::Error, "Unexpected token: \"%t\"", bad_token);
        }

        return token_vector;
    }
}

// TODO: loop continue & loop break
// TODO: restructure next & depth system. attach operations to tokens again?
// TODO: maybe separate compile into multi_compile for patterns with many operations
// TODO: ternary operator needs testing