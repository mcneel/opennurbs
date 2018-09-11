#include "opennurbs.h"

#if !defined(ON_COMPILING_OPENNURBS)
// This check is included in all opennurbs source .c and .cpp files to insure
// ON_COMPILING_OPENNURBS is defined when opennurbs source is compiled.
// When opennurbs source is being compiled, ON_COMPILING_OPENNURBS is defined 
// and the opennurbs .h files alter what is declared and how it is declared.
#error ON_COMPILING_OPENNURBS must be defined when compiling opennurbs
#endif

ON_FixedSizePool::ON_FixedSizePool()
{
  memset(this,0,sizeof(*this));
}

ON_FixedSizePool::~ON_FixedSizePool()
{
  Destroy();
}

#if defined(ON_HAS_RVALUEREF)

ON_FixedSizePool::ON_FixedSizePool(ON_FixedSizePool&& src)
  : m_first_block(src.m_first_block)
  , m_al_element_stack(src.m_al_element_stack)
  , m_al_block(src.m_al_block)
  , m_al_element_array(src.m_al_element_array)
  , m_al_count(src.m_al_count)
  , m_sizeof_element(src.m_sizeof_element)
  , m_block_element_count(src.m_block_element_count)
  , m_active_element_count(src.m_active_element_count)
  , m_total_element_count(src.m_total_element_count)
{
  memset(&src,0,sizeof(*this));
}

ON_FixedSizePool& ON_FixedSizePool::operator=(ON_FixedSizePool&& src)
{
  if (this != &src)
  {
    Destroy();
    
    m_first_block = src.m_first_block;
    m_al_element_stack = src.m_al_element_stack;
    m_al_block = src.m_al_block;
    m_al_element_array = src.m_al_element_array;
    m_al_count = src.m_al_count;
    m_sizeof_element = src.m_sizeof_element;
    m_block_element_count = src.m_block_element_count;
    m_active_element_count = src.m_active_element_count;
    m_total_element_count = src.m_total_element_count;

    memset(&src,0,sizeof(*this));
  }
  return *this;
}

#endif


size_t ON_FixedSizePool::SizeofElement() const
{
  return m_sizeof_element;
}


bool ON_FixedSizePool::Create( 
        size_t sizeof_element, 
        size_t element_count_estimate,
        size_t block_element_capacity
        )
{
  if ( sizeof_element <= 0 )
  {
    ON_ERROR( "ON_FixedSizePool::Create - sizeof_element <= 0" );
    return false;
  }

  if ( m_sizeof_element != 0 || 0 != m_first_block )
  {
    ON_ERROR( "ON_FixedSizePool::Create - called on a pool that is in use." );
    return false;
  }

  memset(this,0,sizeof(*this));

  m_sizeof_element = sizeof_element;

  if ( block_element_capacity <= 0 )
  {
    size_t page_size = ON_MemoryPageSize();
    if ( page_size < 512 )
      page_size = 512;

    // The "overhead" is for the 2*sizeof(void*) ON_FixedSizePool uses at
    // the start of each block + 32 bytes extra for the heap manager
    // to keep the total allocation not exceeding multiple of page_size.
    const size_t overhead = 2*sizeof(void*) + 32;

    size_t page_count = 1;
    block_element_capacity = (page_count*page_size - overhead)/m_sizeof_element;
    while ( block_element_capacity < 1000 )
    {
      page_count *= 2;
      block_element_capacity = (page_count*page_size - overhead)/m_sizeof_element;
      if (page_count > 8 && block_element_capacity > 64)
      {
        // for pools with large elements
        break;
      }
    }
  }

  // capacity for the the 2nd and subsequent blocks
  m_block_element_count = block_element_capacity;

  // Set m_al_count = capacity of the first block.

  // If the estimated number of elements is not too big, 
  // then make the first block that size.
  if ( element_count_estimate > 0 )
  {
    // this is the first block and it has a custom size
    if ( 8*m_block_element_count >= element_count_estimate )
      m_al_count = element_count_estimate;
    else 
      m_al_count = 8*m_block_element_count; // first block will be large
  }
  else
  {
    m_al_count = m_block_element_count;
  }

  return true;
}

void ON_FixedSizePool::ReturnAll()
{
  if ( 0 != m_first_block )
  {
    // initialize
    m_al_element_stack = 0;
    //////m_qwerty_it_block = 0;
    //////m_qwerty_it_element = 0;
    m_al_block = m_first_block;
    m_al_element_array = (void*)(((char*)m_al_block) + 2*sizeof(void*));
    m_al_count = BlockElementCapacity(m_first_block); 
    m_active_element_count = 0;
    m_total_element_count = 0;
  }
}

void ON_FixedSizePool::Destroy()
{
  void* p;
  void* next;
  next = m_first_block;
  memset(this,0,sizeof(*this));
  for ( p = next; 0 != p; p = next )
  {
    next = *((void**)p);
    onfree(p);
  }
}

size_t ON_FixedSizePool::ActiveElementCount() const
{
  return m_active_element_count;
}

size_t ON_FixedSizePool::TotalElementCount() const
{
  return m_total_element_count;
}

//static void DebugIsValid(const ON_FixedSizePool* p)
//{
//  if( !p->IsValid() )
//    p->IsValid();
//}

void* ON_FixedSizePool::AllocateDirtyElement()
{
  //static unsigned int debug_count = 0;
  //debug_count++;
  //const unsigned int debug_count_mark = 21432;
  //if (21432 == debug_count)
  //{
  //  // A conditional breakpoint is too slow for large values of debug_count
  //  debug_count = debug_count_mark; // <- breakpoint here
  //}

  //DebugIsValid(this);

  void* p;  

  if ( 0 != m_al_element_stack )
  {
    // use item on the returned stack first.
    p = m_al_element_stack;
    m_al_element_stack = *((void**)m_al_element_stack);
    //DebugIsValid(this);
  }
  else
  {
    if ( 0 == m_al_block || 0 == m_al_count )
    {
      // No more memory left in m_al_block.
      void* next_block = (0 != m_al_block)
                       ? *((void**)m_al_block)
                       : 0;
      if ( 0 == next_block )
      {
        // This if clause is used when we need to allocate a new block from the heap
        if ( 0 == m_sizeof_element )
        {
          ON_ERROR("ON_FixedSizePool::AllocateElement - you must call ON_FixedSizePool::Create with a valid element size before using ON_FixedSizePool");
          return nullptr;
        }
        // allocate a new block
        if ( 0 == m_al_count )
          m_al_count = m_block_element_count;

        if ( m_al_count <= 0 )
        {
          ON_ERROR("ON_FixedSizePool::AllocateElement - you must call ON_FixedSizePool::Create with a valid element size before using ON_FixedSizePool");
          return nullptr;
        }

        p = onmalloc( 2*sizeof(void*) + m_al_count*m_sizeof_element ); // get some heap

        // set "next" pointer to zero
        *((void**)p) = nullptr;

        // set "end" pointer to address after last byte in the block
        *((void**)(((char*)p) + sizeof(void*))) = ((char*)p) + (2*sizeof(void*) + m_al_count*m_sizeof_element);
        if ( 0 == m_first_block )
        {
          m_first_block = p;
          // If the call to Create() specified a positive element_count_estimate,
          // then m_sizeof_block needs to be reset for any future block allocations.
          
        }
        else
        {
          // If m_first_block != 0, then m_al_block is nonzero (or memory for this class has been trashed)
          *((void**)m_al_block) = p;
        }
        m_al_block = p;
      }
      else
      {
        // If we get here, ReturnAll() was used at some point in
        // the past, m_al_block != 0, m_al_count = zero, and we are
        // reusing blocks that were allocated early.
        m_al_block = next_block;
        m_al_count = BlockElementCapacity(m_al_block);
      }

      m_al_element_array = (void*)(((char*)m_al_block)+2*sizeof(void*));
      //DebugIsValid(this);
    }
    m_al_count--;
    p = m_al_element_array;
    m_al_element_array = (void*)(((char*)m_al_element_array) + m_sizeof_element);
    m_total_element_count++;
  }

  m_active_element_count++;

  //DebugIsValid(this);

  return p;
}

bool ON_FixedSizePool::IsValid() const
{
  if (nullptr != m_first_block)
  {
    const char* block;
    const char* block_end;
    const char* next_block;
    size_t sizeof_block_allocated;
    size_t sizeof_block_total;
    size_t block_element_capacity;
    size_t block_element_count; // allocated element count
    size_t count, capacity;

    size_t total_element_count = 0;

    bool bSkipCcountCheck = false;

    for (block = (const char*)m_first_block; 0 != block; block = next_block)
    {
      const bool bBlockIsAlBlock = (block == m_al_block);

      capacity = BlockElementCapacity(block);
      count 
        = bSkipCcountCheck
        ? 0xFFFFFFFF :
        BlockElementCount(block);

      // validate capacity
      next_block = *((const char**)block);
      block += sizeof(void*);
      block_end = *((const char**)(block));
      block += sizeof(void*);
      sizeof_block_total = (block_end - block);

      block_element_capacity = sizeof_block_total / m_sizeof_element;
      if (sizeof_block_total != block_element_capacity * m_sizeof_element)
      {
        ON_ERROR("sizeof_block is not a multiple of m_sizeof_element");
        return false;
      }

      if (capacity != block_element_capacity)
      {
        ON_ERROR("ON_FixedSizePool::BlockElementCapacity error.");
        return false;
      }

      if ( bSkipCcountCheck )
        continue;

      bSkipCcountCheck = bBlockIsAlBlock;

      // Validate allocated count

      if (bBlockIsAlBlock)
      {
        sizeof_block_allocated = (((const char*)m_al_element_array) - block);
        block_element_count = sizeof_block_allocated / m_sizeof_element;
        if (sizeof_block_allocated != block_element_count * m_sizeof_element)
        {
          ON_ERROR("sizeof_block_allocated is not a multiple of m_sizeof_element");
          return false;
        }
        if ( block_element_count > block_element_capacity )
        {
          ON_ERROR("block_element_count > block_element_capacity");
          return false;
        }
        if ( block_element_count + m_al_count != block_element_capacity)
        {
          ON_ERROR("block_element_count + m_al_count != block_element_capacity");
          return false;
        }
      }
      else
      {
        sizeof_block_allocated = sizeof_block_total;
        block_element_count = block_element_capacity;
      }
      
      total_element_count += block_element_count;
      if (total_element_count > m_total_element_count)
      {
        ON_ERROR("m_total_element_count is not correct or some other serious problem.");
        return false;
      }

      if (count != block_element_count)
      {
        ON_ERROR("ON_FixedSizePool::BlockElementCount error.");
        return false;
      }
    }

    if (total_element_count != m_total_element_count)
    {
      ON_ERROR("m_total_element_count or m_al_element_array is not correct or some other serious problem.");
      return false;
    }
  }

  if ( m_active_element_count > m_total_element_count )
  {
    ON_ERROR("m_active_element_count > m_total_element_count");
    return false;
  }


  return true;
}


void* ON_FixedSizePool::AllocateElement()
{
  void* p = AllocateDirtyElement();
  if (nullptr != p)
    memset(p, 0, m_sizeof_element);
  return p;
}

void ON_FixedSizePool::ReturnElement(void* p)
{
  if ( p )
  {
    if ( m_active_element_count <= 0 )
    {
      // If you get this error, something is seriously wrong.
      // You may be returning the same element multiple times or 
      // you may be returning pointers that are not from this pool.
      // In any case, you're probably going to be crashing sometime soon.
      ON_ERROR("ON_FixedSizePool::ReturnElement - no active elements exist.");
    }
    else
    {
      m_active_element_count--;
      *((void**)p) = m_al_element_stack;
      m_al_element_stack = p;
    }
  }
}


ON_FixedSizePoolIterator::ON_FixedSizePoolIterator()
  : m_fsp(0)
  , m_it_block(0)
  , m_it_element(0)
{}

ON_FixedSizePoolIterator::ON_FixedSizePoolIterator( const ON_FixedSizePool& fsp )
  : m_fsp(&fsp)
  , m_it_block(0)
  , m_it_element(0)
{}

const class ON_FixedSizePool* ON_FixedSizePoolIterator::FixedSizePool()
{
  return m_fsp;
}

void ON_FixedSizePoolIterator::Create(const ON_FixedSizePool* fsp)
{
  m_fsp = fsp;
  m_it_block = 0;
  m_it_element = 0;
}


void ON_FixedSizePoolIterator::Reset()
{
  m_it_block = 0;
  m_it_element = 0;
}

//////void* ON_FixedSizePool::FirstElement()
//////{
//////  if ( m_first_block && m_total_element_count > 0 )
//////  {
//////    m_qwerty_it_block = m_first_block;
//////    m_qwerty_it_element = (void*)(((char*)m_qwerty_it_block)+2*sizeof(void*)); // m_qwerty_it_element points to first element in m_first_block
//////  }
//////  else
//////  {
//////    m_qwerty_it_block = 0;
//////    m_qwerty_it_element = 0;
//////  }
//////  return m_qwerty_it_element;
//////}

void* ON_FixedSizePoolIterator::FirstElement()
{
  if ( m_fsp && m_fsp->m_first_block && m_fsp->m_total_element_count > 0 )
  {
    m_it_block = m_fsp->m_first_block;
    m_it_element = (void*)(((char*)m_it_block)+2*sizeof(void*)); // m_it_element points to first element in m_first_block
  }
  else
  {
    m_it_block = 0;
    m_it_element = 0;
  }
  return m_it_element;
}

//////void* ON_FixedSizePool::NextElement()
//////{
//////  if ( m_qwerty_it_element )
//////  {
//////    m_qwerty_it_element = (void*)(((char*)m_qwerty_it_element) + m_sizeof_element);
//////    if ( m_qwerty_it_element == m_al_element_array )
//////    {
//////      m_qwerty_it_block = 0;
//////      m_qwerty_it_element = 0;
//////    }
//////    else if ( m_qwerty_it_element == *((void**)(((char*)m_qwerty_it_block) + sizeof(void*))) )
//////    {
//////      // m_qwerty_it_element  = "end" pointer which means we are at the end of m_qwerty_it_block
//////      m_qwerty_it_block = *((void**)m_qwerty_it_block); // m_qwerty_it_block = "next" block
//////      m_qwerty_it_element = (0 != m_qwerty_it_block)    // m_qwerty_it_element points to first element in m_qwerty_it_block
//////                   ? (void*)(((char*)m_qwerty_it_block)+2*sizeof(void*))
//////                   : 0;
//////      if ( m_qwerty_it_element == m_al_element_array )
//////      {
//////        m_qwerty_it_block = 0;
//////        m_qwerty_it_element = 0;
//////      }
//////    }
//////  }
//////  return m_qwerty_it_element;
//////}

void* ON_FixedSizePoolIterator::NextElement()
{
  if ( m_it_element )
  {
    m_it_element = (void*)(((char*)m_it_element) + m_fsp->m_sizeof_element);
    if ( m_it_element == m_fsp->m_al_element_array )
    {
      m_it_block = (void*)1; // must be non-zero
      m_it_element = 0; // terminates iteration
    }
    else if ( m_it_element == *((void**)(((char*)m_it_block) + sizeof(void*))) )
    {
      // m_it_element  = "end" pointer which means we are at the end of m_it_block
      m_it_block = *((void**)m_it_block); // m_it_block = "next" block
      m_it_element = (0 != m_it_block)    // m_it_element points to first element in m_it_block
                   ? (void*)(((char*)m_it_block)+2*sizeof(void*))
                   : 0;
      if ( m_it_element == m_fsp->m_al_element_array )
      {
        // terminate iteration (
        m_it_block = (void*)1; // must be non-zero
        m_it_element = 0; // terminates iteration
      }
    }
  }
  else if ( 0 == m_it_block )
  {
    // Start at the beginning.
    FirstElement();
  }
  return m_it_element;
}

void* ON_FixedSizePoolIterator::CurrentElement() const
{
  return m_it_element;
}

//////void* ON_FixedSizePool::FirstElement(size_t element_index)
//////{
//////  const char* block;
//////  const char* block_end;
//////  const char* next_block;
//////  size_t block_count;
//////
//////  m_qwerty_it_block = 0;
//////  m_qwerty_it_element = 0;
//////  if ( element_index < m_total_element_count )
//////  {
//////    for ( block = (const char*)m_first_block; 0 != block; block = next_block )
//////    {
//////      if ( block == m_al_block )
//////      {
//////        next_block = 0;
//////        block_end = (const char*)m_al_element_array;
//////      }
//////      else
//////      {
//////        next_block = *((const char**)block);
//////        block_end =  *((const char**)(block + sizeof(void*)));
//////      }
//////      block_count = (block_end - block)/m_sizeof_element;
//////      if ( element_index < block_count )
//////      {
//////        m_qwerty_it_block = (void*)block;
//////        m_qwerty_it_element = ((void*)(block + (2*sizeof(void*) + element_index*m_sizeof_element)));
//////        break;
//////      }
//////      element_index -= block_count;
//////    }
//////  }
//////  return m_qwerty_it_element;
//////}

void* ON_FixedSizePoolIterator::FirstElement(size_t element_index)
{
  const char* block;
  const char* block_end;
  const char* next_block;
  size_t block_count;

  m_it_block = 0;
  m_it_element = 0;
  if ( m_fsp && element_index < m_fsp->m_total_element_count )
  {
    for ( block = (const char*)m_fsp->m_first_block; 0 != block; block = next_block )
    {
      if ( block == m_fsp->m_al_block )
      {
        next_block = 0;
        block_end = (const char*)m_fsp->m_al_element_array;
      }
      else
      {
        next_block = *((const char**)block);
        block_end =  *((const char**)(block + sizeof(void*)));
      }
      block_count = (block_end - block)/m_fsp->m_sizeof_element;
      if ( element_index < block_count )
      {
        m_it_block = (void*)block;
        m_it_element = ((void*)(block + (2*sizeof(void*) + element_index*m_fsp->m_sizeof_element)));
        break;
      }
      element_index -= block_count;
    }
  }
  return m_it_element;
}

size_t ON_FixedSizePool::BlockElementCapacity( const void* block ) const
{
  // returns number of items that can be allocated from block
  if ( 0 == block || m_sizeof_element <= 0 )
    return 0;

  const char* block_end = *((const char**)(((const char*)block)+sizeof(void*)));
  const char* block_head = (((const char*)block) + 2*sizeof(void*));
  return (block_end - block_head)/m_sizeof_element;
}

size_t ON_FixedSizePool::BlockElementCount( const void* block ) const
{
  // returns number of items currently allocated from block
  if ( 0 == block || m_sizeof_element <= 0 )
    return 0;

  const char* block_end 
    = (block == m_al_block && m_al_count > 0)
    ? ((const char*)m_al_element_array)
    : *((const char**)(((const char*)block)+sizeof(void*)));

  const char* block_head = (((const char*)block) + 2*sizeof(void*));

  return (block_end - block_head)/m_sizeof_element;
}

//////void* ON_FixedSizePool::FirstBlock( size_t* block_element_count )
//////{
//////  if ( m_first_block && m_total_element_count > 0 )
//////  {
//////    m_qwerty_it_block = m_first_block;
//////    m_qwerty_it_element = (void*)(((char*)m_qwerty_it_block)+2*sizeof(void*)); // m_qwerty_it_element points to first element in m_first_block
//////    if ( 0 != block_element_count )
//////      *block_element_count = BlockElementCount(m_qwerty_it_block);
//////  }
//////  else
//////  {
//////    m_qwerty_it_block = 0;
//////    m_qwerty_it_element = 0;
//////    if ( 0 != block_element_count )
//////      *block_element_count = 0;
//////  }
//////  return m_qwerty_it_element;
//////}

void* ON_FixedSizePoolIterator::FirstBlock( size_t* block_element_count )
{
  if ( m_fsp && m_fsp->m_first_block && m_fsp->m_total_element_count > 0 )
  {
    m_it_block = m_fsp->m_first_block;
    m_it_element = (void*)(((char*)m_it_block)+2*sizeof(void*)); // m_it_element points to first element in m_first_block
    if ( 0 != block_element_count )
      *block_element_count = m_fsp->BlockElementCount(m_it_block);
  }
  else
  {
    m_it_block = 0;
    m_it_element = 0;
    if ( 0 != block_element_count )
      *block_element_count = 0;
  }
  return m_it_element;
}

//////void* ON_FixedSizePool::NextBlock( size_t* block_element_count )
//////{
//////  if ( 0 != m_qwerty_it_block 
//////       && m_qwerty_it_block != m_al_block
//////       && m_qwerty_it_element == (void*)(((char*)m_qwerty_it_block)+2*sizeof(void*)) )
//////  {
//////    m_qwerty_it_block = *((void**)m_qwerty_it_block);
//////    if ( m_qwerty_it_block == m_al_element_array )
//////    {
//////      m_qwerty_it_block = 0;
//////      m_qwerty_it_element = 0;
//////      if ( 0 != block_element_count )
//////        *block_element_count = 0;
//////    }
//////    else
//////    {
//////      m_qwerty_it_element = (void*)(((char*)m_qwerty_it_block)+2*sizeof(void*)); // m_qwerty_it_element points to first element in m_first_block
//////      if ( 0 != block_element_count )
//////        *block_element_count = BlockElementCount(m_qwerty_it_block);
//////    }
//////  }
//////  else
//////  {
//////    m_qwerty_it_block = 0;
//////    m_qwerty_it_element = 0;
//////    if ( 0 != block_element_count )
//////      *block_element_count = 0;
//////  }
//////  return m_qwerty_it_element;
//////}

void* ON_FixedSizePoolIterator::NextBlock( size_t* block_element_count )
{
  if ( 0 != m_it_block 
       && m_it_block != m_fsp->m_al_block
       && m_it_element == (void*)(((char*)m_it_block)+2*sizeof(void*)) )
  {
    m_it_block = *((void**)m_it_block);
    if ( m_it_block == m_fsp->m_al_element_array )
    {
      m_it_block = 0;
      m_it_element = 0;
      if ( 0 != block_element_count )
        *block_element_count = 0;
    }
    else
    {
      m_it_element = (void*)(((char*)m_it_block)+2*sizeof(void*)); // m_it_element points to first element in m_first_block
      if ( 0 != block_element_count )
        *block_element_count = m_fsp->BlockElementCount(m_it_block);
    }
  }
  else
  {
    m_it_block = 0;
    m_it_element = 0;
    if ( 0 != block_element_count )
      *block_element_count = 0;
  }
  return m_it_element;
}

void* ON_FixedSizePool::Element(size_t element_index) const
{
  if (element_index < m_total_element_count)
  {
    const char* block;
    const char* block_end;
    const char* next_block;
    size_t block_count;

    for (block = (const char*)m_first_block; 0 != block; block = next_block)
    {
      if (block == m_al_block)
      {
        next_block = nullptr;

        // for debugging
        // block += sizeof(void*);
        // block_end = *((const char**)(block));
        // block += sizeof(void*);

        block_end = (const char*)m_al_element_array;
        block += 2*sizeof(void*);
      }
      else
      {
        next_block = *((const char**)block);
        block += sizeof(void*);
        block_end = *((const char**)(block));
        block += sizeof(void*);
      }
      block_count = (block_end - block) / m_sizeof_element;
      if (element_index < block_count)
        return ((void*)(block + element_index*m_sizeof_element));
      element_index -= block_count;
    }
  }

  return nullptr;
}

size_t ON_FixedSizePool::ElementIndex(const void* element_pointer) const
{
  if (nullptr != element_pointer)
  {
    const char* block;
    const char* block_end;
    const char* next_block;
    size_t block_count;
    const char* ptr = (const char*)element_pointer;
    size_t ptr_index = 0;
    for (block = (const char*)m_first_block; 0 != block; block = next_block)
    {
      if (block == m_al_block)
      {
        // After a ReturnAll(), a multi-block fsp has unused blocks after m_al_block.
        // Searching must terminate at m_al_block.
        next_block = nullptr;
        block_end = (const char*)m_al_element_array;
        block += (2 * sizeof(void*));
      }
      else
      {
        next_block = *((const char**)block);
        block += sizeof(void*);
        block_end = *((const char**)(block));
        block += sizeof(void*);
      }
      if (ptr >= block && ptr < block_end)
      {
        size_t offset = ptr - block;
        if (0 == offset % m_sizeof_element)
        {
          ptr_index += (unsigned int)(offset/m_sizeof_element);
          return ptr_index;
        }
        // Caller is confused
        ON_ERROR("element_pointer is offset into an fsp element.");
        return ON_MAX_SIZE_T;
      }
      block_count = (block_end - block) / m_sizeof_element;
      ptr_index += block_count;
    }
    // Caller is confused
    ON_ERROR("element_pointer is not in allocated fsp memory.");
    return ON_MAX_SIZE_T;
  }

  return ON_MAX_SIZE_T;
}


void* ON_FixedSizePool::ElementFromId(
  size_t id_offset,
  unsigned int id
  ) const
{
  const char* block;
  const char* block_end;
  const char* next_block;
  unsigned int i0, i1;
  size_t count;
  if (id_offset + sizeof(id) > m_sizeof_element)
  {
    // caller is confused.
    ON_ERROR("id_offset is too large.");
    return nullptr;
  }

  for (block = (const char*)m_first_block; 0 != block; block = next_block)
  {
    if (block == m_al_block)
    {
      next_block = nullptr;
      block_end = (const char*)m_al_element_array;
      block += (2 * sizeof(void*));
    }
    else
    {
      next_block = *((const char**)block);
      block += sizeof(void*);
      block_end = *((const char**)(block));
      block += sizeof(void*);
    }

    i1 = *((const unsigned int*)(block_end-(m_sizeof_element-id_offset)));
    if (i1 < id)
      continue;

    if ( id == i1 )
      return (void*)(block_end-m_sizeof_element);

    i0 = *((const unsigned int*)(block + id_offset));
    if (id < i0)
      continue;

    if ( id == i0 )
      return (void*)(block);

    count = (block_end - block)/m_sizeof_element;
    if (i1 - i0 + 1 == count)
    {
      return (void*)(block + ((id-i0)*m_sizeof_element));
    }

    return (void*)ON_BinarySearchArrayForUnsingedInt(id, block, count, m_sizeof_element, id_offset );
  }

  return nullptr;
}

