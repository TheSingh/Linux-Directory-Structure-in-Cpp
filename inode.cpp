// $Id: inode.cpp,v 1.12 2014-07-03 13:29:57-07 - - $

#include <algorithm>
#include <iostream>
#include <stdexcept>

using namespace std;

#include "debug.h"
#include "inode.h"

int inode::next_inode_nr {1};

inode::inode(inode_t init_type):
  inode_nr (next_inode_nr++), type (init_type)
{
   switch (type) {
      case PLAIN_INODE:
           contents = make_shared<plain_file>();
           break;
      case DIR_INODE:
           contents = make_shared<directory>();
           break;
   }
   DEBUGF ('i', "inode " << inode_nr << ", type = " << type);
}

int inode::get_inode_nr() const {
   DEBUGF ('i', "inode = " << inode_nr);
   return inode_nr;
}

plain_file_ptr plain_file_ptr_of (file_base_ptr ptr) {
   plain_file_ptr pfptr = dynamic_pointer_cast<plain_file> (ptr);
   if (pfptr == nullptr) throw invalid_argument ("plain_file_ptr_of");
   return pfptr;
}

directory_ptr directory_ptr_of (file_base_ptr ptr) {
   directory_ptr dirptr = dynamic_pointer_cast<directory> (ptr);
   if (dirptr == nullptr) throw invalid_argument ("directory_ptr_of");
   return dirptr;
}


size_t plain_file::size() const {
   size_t size {0};
   DEBUGF ('i', "size = " << size);
   return size;
}

const wordvec& plain_file::readfile() const {
   DEBUGF ('i', data);
   return data;
}

void plain_file::writefile (const wordvec& words) {
   DEBUGF ('i', words);
   data = words;
}

size_t directory::size() const {
   size_t size {0};
   DEBUGF ('i', "size = " << size);
   return size;
}

void directory::remove (const string& filename) {
   DEBUGF ('i', filename);
}

inode& directory::mkdir (inode_state &state, const string& filename) {
  DEBUGF ('i', filename);
  string instr = filename;
  this->dirents.insert(pair<string, shared_ptr<inode>>(instr, make_shared<inode>(inode(DIR_INODE))));
  directory_ptr_of((*(this->dirents.find(instr)->second)).getContents())->setParent(directory_ptr_of((*state.getCwd()).getContents()));
  map<string, inode_ptr> mp = {};
  directory_ptr_of((*(this->dirents.find(instr)->second)).getContents())->setDirents(mp);
  return *(this->dirents.at(instr)).get();
}

inode& directory::mkfile (const string& filename) {
  DEBUGF ('i', filename);
  string instr = filename;
  this->dirents.insert(pair<string, shared_ptr<inode>>(instr, make_shared<inode>(inode(PLAIN_INODE))));
  return *(this->dirents.at(instr)).get();
}

wordvec directory::getDirents() {
  DEBUGF ('i', "fetching directory entries");
  wordvec wv = {};
  for(auto it = dirents.begin(); it!= dirents.end(); ++it) {
    wv.push_back(it->first);
  }
  sort(wv.begin(), wv.end());
  return wv;
}

map<string, inode_ptr>& directory::getMap() {
  DEBUGF ('i', "fetching map");
  return dirents;
}

inode_state::inode_state() {
  root =  make_shared<inode>(DIR_INODE);
  cwd = root;
  path = wordvec();
  DEBUGF ('i', "root = " << root << ", cwd = " << cwd
          << ", prompt = \"" << prompt << "\"");  
}

ostream& operator<< (ostream& out, const inode_state& state) {
   out << "inode_state: root = " << state.root
       << ", cwd = " << state.cwd;
   return out;
}

