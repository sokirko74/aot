cmake_minimum_required(VERSION 3.5)
project (SimpleGrammarGerman)
CompileSimpleGrammar(ExampleGLRSynan example.grm example.table)
CompileSimpleGrammar(PersonGLRSynan person.grm person.table)
