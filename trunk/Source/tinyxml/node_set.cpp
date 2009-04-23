/*
www.sourceforge.net/projects/tinyxpath
Copyright (c) 2002-2004 Yves Berquin (yvesb@users.sourceforge.net)

This software is provided 'as-is', without any express or implied
warranty. In no event will the authors be held liable for any
damages arising from the use of this software.

Permission is granted to anyone to use this software for any
purpose, including commercial applications, and to alter it and
redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must
not claim that you wrote the original software. If you use this
software in a product, an acknowledgment in the product documentation
would be appreciated but is not required.

2. Altered source versions must be plainly marked as such, and
must not be misrepresented as being the original software.

3. This notice may not be removed or altered from any source
distribution.
*/

#include "node_set.h"

namespace TinyXPath
{

/// Copy constructor
node_set::node_set (const node_set & ns2)
{  
   * this = ns2;
}

/// Assignation operator. Allows one to write expressions like ns_1 = ns_2;
node_set & node_set::operator = (const node_set & ns2)
{
   u_nb_node = ns2 . u_nb_node;
   if (u_nb_node)
   {
      vpp_node_set = new const void * [u_nb_node];
      memcpy (vpp_node_set, ns2 . vpp_node_set, u_nb_node * sizeof (void *));
      op_attrib = new bool [u_nb_node];
      memcpy (op_attrib, ns2 . op_attrib, u_nb_node * sizeof (bool));
   }
   else
   {
      vpp_node_set = NULL;
      op_attrib = NULL;
   }
   return * this;
}

/// Copy all element children of a node to the node_set
void node_set::v_copy_node_children (
   const TiXmlNode * XNp_root)   ///< The father of the nodes to be copied
{
   v_copy_node_children (XNp_root, NULL);
}

/// Copy all element children of a node to the node_set, if their name matches a given name
void node_set::v_copy_node_children (
   const TiXmlNode * XNp_root,   ///< The father of the nodes to be copied
   const char * cp_lookup)       ///< Lookup name (or NULL)
{
   const TiXmlNode * XNp_child;

   XNp_child = XNp_root -> FirstChildElement ();
   while (XNp_child)
   {
      if ((! cp_lookup) || ! strcmp (XNp_child -> Value (), cp_lookup))
         v_add_node_in_set (XNp_child);
      XNp_child = XNp_child -> NextSiblingElement ();
   }
}

/// Copy all nodes in the tree to the node_set
void node_set::v_copy_selected_node_recursive (
   const TiXmlNode * XNp_root)      ///< The node to be copied
{
   v_copy_selected_node_recursive (XNp_root, NULL);
}

/// Copy all nodes in the tree to the node_set
void node_set::v_copy_selected_node_recursive (
   const TiXmlNode * XNp_root,      ///< The node to be copied
   const char * cp_lookup)          ///< Lookup name (or NULL)
{
   TiXmlAttribute * XAp_attrib;
   const TiXmlNode * XNp_child;

   if ((! cp_lookup) || ! strcmp (XNp_root -> Value (), cp_lookup))
      v_add_node_in_set (XNp_root);
   if (XNp_root -> Type () == TiXmlNode::ELEMENT)
   {
      XAp_attrib = XNp_root -> ToElement () -> FirstAttribute ();      
      while (XAp_attrib)
      {  
         v_add_attrib_in_set (XAp_attrib);
         XAp_attrib = XAp_attrib -> Next ();
      }
   }
   XNp_child = XNp_root -> FirstChild ();
   while (XNp_child)
   {
      v_copy_selected_node_recursive (XNp_child, cp_lookup);
      XNp_child = XNp_child -> NextSiblingElement ();
   }
}

/// Copy all nodes in the tree to the node_set, excluding attributes
void node_set::v_copy_selected_node_recursive_no_attrib (
   const TiXmlNode * XNp_root,   ///< Node whole children are to be copied
   const char * cp_lookup)       ///< Lookup name or NULL
{
   const TiXmlElement * XEp_child;

   XEp_child = XNp_root -> FirstChildElement ();
   while (XEp_child)
   {
      if ((! cp_lookup) || ! strcmp (XEp_child -> Value (), cp_lookup))
         v_add_node_in_set (XEp_child);
      v_copy_selected_node_recursive_no_attrib (XEp_child, cp_lookup);
      XEp_child = XEp_child -> NextSiblingElement ();
   }
}

/// Return the string value aka concatenation of all text items
TIXML_STRING node_set::S_get_string_value () const
{
   TIXML_STRING S_res;
   const TiXmlNode * XNp_node;
   unsigned u_node;

   S_res = "";
   for (u_node = 0; u_node < u_nb_node; u_node++)
   {
      if (! op_attrib [u_node])
      {
         XNp_node = (const TiXmlNode *) vpp_node_set [u_node];
         if (XNp_node -> Type () == TiXmlNode::TEXT)
            S_res += XNp_node -> Value ();
      }
   }
   return S_res;
}

/// Checks if a node exist in the node set
bool node_set::o_exist_in_set (
   const TiXmlBase * XBp_member)    ///< Check if a base exist in the node set
{
   unsigned u_node;

   for (u_node = 0; u_node < u_nb_node; u_node++)
      if (vpp_node_set [u_node] == XBp_member)
         return true;
   return false;
}

/// Adds a new node in the node set
void node_set::v_add_base_in_set (
   const TiXmlBase * XBp_member,    ///< Base to add (node or attribute)
   bool o_attrib)                   ///< true if the base is an attribute, false if it's a node
{
   const void ** vpp_new_list;
   bool * op_new_list;

   if (o_exist_in_set (XBp_member))
      return;
   vpp_new_list = new const void * [u_nb_node + 1];
   op_new_list = new bool [u_nb_node + 1];
   if (u_nb_node)
   {
      memcpy (vpp_new_list, vpp_node_set, u_nb_node * sizeof (void *));
      delete [] vpp_node_set;
      memcpy (op_new_list, op_attrib, u_nb_node * sizeof (bool));
      delete [] op_attrib;
   }
   vpp_new_list [u_nb_node] = (const void *) XBp_member;
   vpp_node_set = vpp_new_list;
   op_new_list [u_nb_node] = o_attrib;
   op_attrib = op_new_list;
   u_nb_node++;
}

/// Populate the node set with all following nodes.
/// \n Exerpt : \n
/// the following axis contains all nodes in the same document as the context 
/// node that are after the context node in document order, excluding any 
/// descendants and excluding attribute nodes and namespace nodes
void node_set::v_add_all_foll_node (
   const TiXmlNode * XNp_node,   ///< base node
   const TIXML_STRING & S_name)  ///< lookup name (or "*")
{
   const TiXmlNode * XNp_ptr;
   const char * cp_lookup;

   if (S_name == "*")
      cp_lookup = NULL;
   else
      cp_lookup = S_name . c_str ();
   XNp_ptr = XNp_node -> NextSiblingElement ();
   while (XNp_ptr)
   {
      v_add_node_in_set_if_name_or_star (XNp_ptr, S_name);
      v_copy_node_children (XNp_ptr, cp_lookup);
      XNp_ptr = XNp_ptr -> NextSiblingElement ();
   }
   XNp_ptr = XNp_node -> Parent ();
   if (XNp_ptr && XNp_ptr -> Type () == TiXmlNode::ELEMENT)
      v_add_all_foll_node (XNp_ptr, S_name);
}

/// Populate the node set with all preceding nodes.
/// \n Exerpt : \n
/// the preceding axis contains all nodes in the same document as the context 
/// node that are before the context node in document order, excluding any 
/// ancestors and excluding attribute nodes and namespace nodes
void node_set::v_add_all_prec_node (
   const TiXmlNode * XNp_node,      ///< base node
   const TIXML_STRING & S_name)     ///< lookup name (or "*")
{
   const TiXmlNode * XNp_ptr;
   const char * cp_lookup;

   if (S_name == "*")
      cp_lookup = NULL;
   else
      cp_lookup = S_name . c_str ();
   XNp_ptr = XNp_node -> PreviousSibling ();
   while (XNp_ptr)
   {
      if (XNp_ptr -> Type () == TiXmlNode::ELEMENT)
      {
         v_add_node_in_set_if_name_or_star (XNp_ptr, S_name);
         v_copy_node_children (XNp_ptr, cp_lookup);
      }
      XNp_ptr = XNp_ptr -> PreviousSibling ();
   }
}

/// Internal utility class for the node set sorting
class ptr_and_flag 
{
public :
   const void * vp_ptr;
   bool o_flag;
} ;

/// Internal utility function for node set sorting
int i_compare_ptr_and_flag (
   const void * vp_1,   ///< Ptr to first element to compare
   const void * vp_2)   ///< Ptr to second element to compare
{
   const ptr_and_flag * pafp_1, * pafp_2;
   TiXmlNode * XNp_1, * XNp_2;

   pafp_1 = (const ptr_and_flag *) vp_1;
   pafp_2 = (const ptr_and_flag *) vp_2;
   if (pafp_1 -> o_flag || pafp_2 -> o_flag)
      // for now
      return 0;
   XNp_1 = (TiXmlNode *) pafp_1 -> vp_ptr;
   XNp_2 = (TiXmlNode *) pafp_2 -> vp_ptr;
   return (long) (XNp_1 -> GetUserData ()) - (long) (XNp_2 -> GetUserData ());
}

/// Sort the node set according to the document order.
/// \n The document order must have been recorded already in the tiny xml user's value
/// \n There's still a problem with the attributes. They aren't covered by the GetUserData / SetUserData
/// yet. If two attributes come from the same element, we have to compute on the fly their relative position
void node_set::v_document_sort ()
{
   ptr_and_flag * pafp_list;
   unsigned u_node;

   if (u_nb_node < 2)
      return;

   // well, next problem is, if we want to make a clean library, we can't use static variables
   // the only way for us to sort the vpp_node_set / op_attrib is to make it the other way up
   pafp_list = new ptr_and_flag [u_nb_node];
   for (u_node = 0; u_node < u_nb_node; u_node++)
   {
      pafp_list [u_node] . vp_ptr = vpp_node_set [u_node];
      pafp_list [u_node] . o_flag = op_attrib [u_node];
   }
   qsort (pafp_list, u_nb_node, sizeof (ptr_and_flag), i_compare_ptr_and_flag);
   for (u_node = 0; u_node < u_nb_node; u_node++)
   {
      vpp_node_set [u_node] = pafp_list [u_node] . vp_ptr;
      op_attrib [u_node] = pafp_list [u_node] . o_flag;
   }
   delete [] pafp_list;
}

/// Debug function to print the content of a node set to stdout
void node_set::v_dump ()
{
   unsigned u_node;
   const TiXmlAttribute * XAp_att;
   const TiXmlNode * XNp_node;

   printf ("-- start node set (%d items) --\n", u_nb_node);
   for (u_node = 0; u_node < u_nb_node; u_node++)
   {
      if (op_attrib [u_node])
      {
         XAp_att = XAp_get_attribute_in_set (u_node);
         printf ("   [%d] : Attribute : %s=%s\n", u_node, XAp_att -> Name (), XAp_att -> Value ());
      }
      else
      {
         XNp_node = XNp_get_node_in_set (u_node);
         printf ("   [%d] : Node : %s\n", u_node, XNp_node -> Value ());
      }
   }
   printf ("-- end node set --\n");
}

}
