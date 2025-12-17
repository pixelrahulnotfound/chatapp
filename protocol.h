#pragma once
#include<string>
// #include<iostream> not needed
inline  bool starts_with(const std::string& s, const std::string& prefix ){
 if(s.size()<prefix.size()) return false;
//  bool flag=true;
//  return s.substr(0,prefix.size()) == prefix; shouldnt use this because of performance issues
for(size_t i=0;i<prefix.size();i++) {
if(s[i]!=prefix[i]) return false;   
}
return true;

}