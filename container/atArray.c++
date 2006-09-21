
#include "atArray.h++"
#include <stdlib.h>
#include <string.h>


#define AT_ARRAY_DEFAULT_CAPACITY  16


atArray::atArray()
{
   // Initialize the array, allocating enough space for the default
   // array capacity
   array_items = 
      (atItem **)calloc(AT_ARRAY_DEFAULT_CAPACITY, sizeof(atItem *));
   current_capacity = AT_ARRAY_DEFAULT_CAPACITY;

   // Start with nothing in the array
   num_entries = 0;
}


atArray::~atArray()
{
   int index;

   // Go through the list and delete the items and entries
   for (index = 0; index < num_entries; index++)
   {
       if (array_items[index] != NULL)
       {
           delete array_items[index];
           array_items[index] = NULL;
       }
   }

   // Free the array
   free(array_items);
}


u_long atArray::getNumEntries()
{
   return num_entries;
}


bool atArray::addEntry(atItem * item)
{
   u_long index;

   // Compute the index at the end of the array, and make sure the array is
   // big enough to handle it
   index = num_entries;
   if (ensureCapacity(index))
   {
      // Go ahead and add the entry
      array_items[index] = item;
      num_entries++;
 
      // Return success
      return true;
   }
   else
   {
      // We can't add the item, return failure
      return false;
   }
}


atItem * atArray::setEntry(u_long index, atItem * item)
{
   atItem * oldItem;

   // See if the given index is within the current array bounds
   if (index < num_entries)
   {
      // Get the item at the given index (if any)
      oldItem = array_items[index];

      // Replace the item at the given index with the given item
      array_items[index] = item;

      // Return the old item (may be NULL)
      return oldItem;
   }
   else
   {
      // The index is outside the current array bounds, so this is
      // effectively just an insert operation.  Check the return value
      // of the insert, and print an error if there was a problem
      if (!insertEntry(index, item))
         notify(AT_WARN, "Unable to set item at array index %d\n", index);

      // Return NULL, since there was no item present at that index before
      return NULL;
   }
}


bool atArray::insertEntry(u_long index, atItem * item)
{
   // See if the given index is within the bounds of the current array
   if (index < num_entries)
   {
      // We need to shift some number of elements in the array to the right.
      // This means we need to make sure the array is big enough to handle the
      // additional element at the end.
      if (ensureCapacity(num_entries+1))
      {
         // We have room, so we need to shift the elements at the index
         // in question, and all subsequent elements, down one space
         memmove(&array_items[index+1], &array_items[index],
            sizeof(atItem *) * (num_entries - index + 1));

         // Add the entry and increment the entry count
         array_items[index] = item;
         num_entries++;
      }
      else
      {
         // We can't get the space to insert the element, return failure
         return false;
      }
   }
   else
   {
      // Make sure the array is big enough to handle the given index
      if (ensureCapacity(index))
      {
         // Add the entry
         array_items[index] = item;

         // Update the entry count.  Since we added an item at an index
         // beyond the current entry count, all the items in between the
         // previous entry count and the new index are also implicitly 
         // filled (even though they're NULL)
         num_entries = index + 1;

         // Return success
         return true;
      }
      else
      {
         // We can't add the item, return failure
         return false;
      }
   }
}


bool atArray::removeEntry(u_long index)
{
   // Make sure the index is valid
   if (index < num_entries)
   {
      // To remove the item, we need to slide any subsequent items to the
      // left to take its place.  First, check to make sure we're not 
      // removing the last item (otherwise, no sliding is necessary)
      if (index < (num_entries - 1))
      {
         // Use a memmove to slide the elements after the given one to the
         // left
         memmove(&array_items[index], &array_items[index+1],
            sizeof(atItem *) * (num_entries - index));

         // Update the entry count
         num_entries--;
      }
   }
   else
   {
      // Print a warning and return failure
      notify(AT_WARN, "The given index is out of bounds\n");
      return false;
   }
}


bool atArray::removeEntry(atItem * item)
{
   long index;

   // Get the index of the given item
   index = getIndexOf(item);

   // See if the index is valid
   if (index > 0)
   {
      // Call removeEntry() with the index of the item we found
      return removeEntry((u_long)index);
   }
   else
   {
      // Print a warning and return failure
      notify(AT_WARN, "The given item could not be found in the array\n");
      return false;
   }
}


bool atArray::removeAllEntries()
{
   // Clear the array to all NULL and set the entry count to zero
   memset(array_items, 0, sizeof(atItem *) * current_capacity);
   num_entries = 0;
}


atItem * atArray::getEntry(u_long index)
{
   // See if the index is within the current array bounds.  Return the
   // specified item if so, or NULL if not.
   if (index < num_entries)
      return array_items[index];
   else
      return NULL;
}


long atArray::getIndexOf(atItem * item)
{
   long index;

   // Start at the beginning of the array, and scan until we find an
   // item in the array that mataches the given item
   index = 0;
   while ((index < num_entries) && (!array_items[index]->equals(item)))
      index++;

   // If we found the item we were looking for, return its index.
   // Otherwise, return -1 to indicate we couldn't find the item.
   if (index < num_entries)
      return index;
   else
      return -1;
}


bool atArray::ensureCapacity(u_long capacity)
{
   u_long newCapacity;
   atItem ** newArray;

   // Check the new capacity against the current capacity
   if (capacity >= current_capacity)
   {
      // The array needs to grow.  Our policy is to double the current
      // capacity.  Using this policy, figure out the new array size so
      // that it is at least as big as the requested capacity
      newCapacity = current_capacity;
      while (capacity > newCapacity)
         newCapacity *= 2;

      // Try to resize the array to the new capacity
      newArray = 
         (atItem **)realloc(array_items, newCapacity * sizeof(atItem *));

      // See if we got a valid array back
      if (newArray == NULL)
      {
          // Reallocation failed, return false to indicate we can't allocate
          // an array of the given capacity
          return false;
      }

      // We've got our new array, update the object member
      array_items = newArray;

      // Initialize all the new memory to NULL, in case someone tries to
      // read from there (which they really shouldn't)
      memset(&array_items[num_entries], 0, 
         sizeof(atItem) * newCapacity - current_capacity);

      // Update the array's capacity indicator
      current_capacity = newCapacity;

      // We've successfully resized the array, so return true
      return true;
   }
   else
   {
      // The array is already as big as the requested capacity.
      // Just return true to indicate that all is well.
      return true;
   }
}