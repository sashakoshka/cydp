/*
  some examples
  
  file manager:
  ---------------------------------
  |            Home         - + x |
  ---------------------------------
  | @ Recent    ^ [ ] [ ] [ ] [ ] |
  | @ Home      #  =   =   =   =  |
  | @ Documents # [ ] [ ] [ ] [ ] |
  | @ Downloads |  =   =   =   =  |
  | @ Music     | [ ] [ ] [ ] [ ] |
  | @ Pictures  V  =   =   =   =  |
  ---------------------------------
  
  rather rough approximation. imagine the @ symbols are icons, the thing in the
  middle is a scrollbar, and the [ ] things with the = under them are files.
  directories with text labels. when a described field is unmentioned, it is
  assumed to be default. this demonstration also includes no controls, this is
  just to describe how lists and layouts work.
  
  there of course would be a non scrollable application window, with the layout
  set to sidebar. that window would have two generic containers as children.
  
  the first generic container would have a vertical list layout, and be
  scrollable, with wrap set to 0. each child would be a clickable generic
  container, with a horizontal list layout. each of these containers would have
  an icon of 32x32 pixels in size, and a text label.
  
  the next generic container would have a horizontal list layout, with wrap set
  to 1. the cell width and height would be dependant on user specified zoom.
  this container would have a number of individual generic containers
  representing individual files. these containers would have a vertical list
  layout and an alignment of 1, and would all be clickable. they would each have
  an icon whose size would be dependant on zoom, and a label text with an
  alignment of 1.
  
  dock:
  --------------------------------------------
  | @  @  @  @  @  @  @  @  @  @  @  @  @  @ |
  --------------------------------------------
  
  this one is rather simple. a generic container with clickable icons for each
  pinned desktop icon. the layout is a scrollable horizontal list. icon size and
  cell size would be matched, and probably based on a user specified value. all
  icons would be draggable as well. the reason for the container being
  scrollable is that when a limit (defined by internal logic) is reached, the
  dock should become scrollable so the user can access all desktop icons pinned
  to it.
*/

#define RTL_READING_DIRECTION 0

typedef struct _El El;

struct _El {
  // 1 bit flags
  unsigned int draggable:1;       // 0 - can be dragged around with mouse
  unsigned int limitDrag:1;       // 0 - cant be dragged outside parent
  
  unsigned int selectable:1;      // 0 - explicitly enable selectability
  unsigned int clickable:1;       /* 0 - use appropritate click/hover styling,
  implies selectability. enabled by default in buttons. */
  
  unsigned int wrap:1;            // 1 - applies to list layout and text

  // only apply to elements that can have children (parent)
  unsigned int scrollable:1;      // 0 - can be scrolled
  unsigned int forceScrollBarX:1; // 0 - always show a horizontal scrollbar
  unsigned int forceScrollBarY:1; // 0 - always show a vertical scrollbar
  
  /*
    type
    determines what sort of element it is. there may be only one root element
    per server. only (parent) elements are capable of having children. this has
    no default value, as it is chosen upon construction.
    
    0: root               (parent)
    1: generic container  (parent)
    2: application window (parent)
    3: text
    4: icon
    5: button
    6: input
    7: raster canvas
    8: vector canvas
  */
  unsigned int type;
  
  /*
    layout
    only applies to elements that can have children (parent). if child is
    draggable, it will be dragged in accordance to this value (eg. being snapped
    to grid, reordering list items).
    
    snapped grid takes the children's positions and rounds it down to a multiple
    of cellWidth and cellHeight, basically snapping it. absolute grid, on the
    other hand, multiplies the child width and height by cellWidth and
    cellHeight.
    
    0: free
    1: snapped grid
    2: absolute grid
    3: horizontal list
    4: vertical list
    5: sidebar
  */
  unsigned int layout;     // 0
  
  /*
    alignment
    applies to lists and elements with text. defaults to 3 for left-to-right
    locales, and 5 for right-to-left locales. defaults to 4 for buttons.
    
    0|1|2
    -+-+-
    3|4|5
    -+-+-
    6|7|8
  */
  unsigned int alignment:3;
  
  /* how big grid cells or list columns/rows are. if zero, the cell dimension is
  automatic. they both default to zero and are only applicable when the layout
  is a snapped gridd, horizontal list, or vertical list. */
  unsigned int cellWidth;  // 0
  unsigned int cellHeight; // 0
  
  /*
    position of the element within the parent container. these values are
    ignored when the layout is something other than free or a grid. these values
    are relative to the origin point 0, 0. however, when these are negative,
    the element position is calculated by adding these values to the parent's
    width and height. thus, they can be anchored from any corner in free or grid
    layouts.
  */
  int x; // 0
  int y; // 0
  
  /*
    each element type has defaults for these. these are ignored when the
    corresponding cell dimension is greater than 0.
    
    when these values are negative, they are treated as a percent. for example,
    -100 would stretch the element.
    
    TODO: implement unit system instead of this
  */
  int width;
  int height;
  
  char *text;  // NULL - for text, buttons, inputs, etc
  int   value; // 0    - for numerical inputs
  
  El *parent;  // pointer to parent element. defaults to the root container.
  El *child;   // NULL - pointer to first child element
  
  El *lSib;    // NULL - pointer to previous (left) sibling
  El *rSib;    // NULL - pointer to next (right) sibling
  
  /* ---------------------------------------------------------------------- +/
  
  these values should only be used by internal draw functions. should not be
  exposed to clients. */
  int calc_x, calc_y, calc_w, calc_h;
};

El *el_make(unsigned int type) {
  El *el = malloc(sizeof(El));
  
  el->draggable = 0;
  el->limitDrag = 0;
  
  el->selectable = 0;
  el->clickable  = 0;
  
  el->wrap            = 1;
  el->scrollable      = 0;
  el->forceScrollBarX = 0;
  el->forceScrollBarY = 0;
  
  el->type      = type;
  el->layout    = 0;
  el->alignment = 3;  // TODO: logic for this
  
  el->cellWidth  = 0;
  el->cellHeight = 0;
  el->x          = 0;
  el->y          = 0;
  
  switch(type) {
    case 0: // root
      el->width  = 640;
      el->height = 480;
      break;
    case 1: // generic container
    case 7: // raster canvas
    case 8: // vector canvas
      el->width  = 0;
      el->height = 0;
      break;
    case 2:
      el->y      = 32;
      el->width  = 512;
      el->height = 384;
      break;
    case 3:
      el->width  = -100;
      el->height = 0;
      break;
    case 4:
      el->width  = 48;
      el->height = 48;
      break;
    case 5:
      el->width  = 96;
      el->height = 32;
      break;
    case 6:
      el->width  = -100;
      el->height = 32;
      break;
  }
  
  el->text  = NULL;
  el->value = 0;
  
  el->parent = NULL;
  el->child  = NULL;
  el->lSib   = NULL;
  el->rSib   = NULL;
  
  return el;
}

El *el_firstSib(El *el) {
  while(el->lSib) el = el->lSib;
  return el;
}

El *el_lastSib(El *el) {
  while(el->rSib) el = el->rSib;
  return el;
}

// return nth sibling relative to an element. return null if not found.
El *el_nSib(El *el, int n) {
  int i;
  for(i = 0; i < n && el->rSib; i++) el = el->rSib;
  if(i == n) return el; // only if this is the correct stop
  else return NULL;
}

// remove a child from its parent and siblings
int el_orphan(El *child) {
  if(child->parent == NULL) return 0;
  if(child->lSib == NULL) { // if this is the first child
    if(child->rSib == NULL) // if this is also the last child
      child->parent->child = NULL;
    else { // we need to account for other children to the right
      child->parent->child = child->rSib;
      child->rSib->lSib = NULL;
    }
  } else if(child->rSib == NULL) { // if this is the last child but not first
    child->lSib->rSib = NULL; // shh... left sibling... you dont know me
  } else {
    child->lSib->rSib = child->rSib; // connect two siblings
    child->rSib->lSib = child->lSib;
  }
  
  // completely orphan element
  child->lSib   = NULL;
  child->rSib   = NULL;
  child->parent = NULL;
  return 1;
}

/* append child to a parent. should only work for orphan element. returns 0 if
the child element was not an orphan. */
int el_adopt(El *parent, El *child) {
  if(child->parent == NULL && child->lSib == NULL && child->rSib == NULL) {
    if(parent->child == NULL) parent->child = child;
    else {
      El *lastSib = el_lastSib(parent->child);
      lastSib->rSib = child;
      child->lSib = lastSib;
    }
    child->parent = parent;
    return 0;
  }
  return 1;
}

/* calculates position and dimensions of all elements relative to root
recursively. this should be run whenever something changes. */
void el_calc(El *el) { // pass in root first!!!!
  if(el->type == 0) {
    el->calc_x = 0;
    el->calc_y = 0;
    el->calc_w = el->width;
    el->calc_h = el->height;
  } else {
    el->calc_x = el->parent->calc_x;
    el->calc_y = el->parent->calc_y;
    
    if(el->width  < 0) el->calc_w
      = (float)el->parent->calc_w * ((float)el->width / -100.0);
    else el->calc_w = el->width;
    
    if(el->height < 0) el->calc_h
      = (float)el->parent->calc_w * ((float)el->height / -100.0);
    else el->calc_h = el->height;
                                    
    // TODO: logic for dealing with pos/dim depending on parent layout
    
    switch(el->parent->layout) {
      case 0: // free
        if(el->x < 0) el->calc_x += el->parent->calc_w + el->x;
        else el->calc_x += el->x;
        
        if(el->y < 0) el->calc_y += el->parent->calc_h + el->y;
        else el->calc_y += el->y;
        break;
      case 1: // snapped grid
        el->calc_x = el->parent->calc_x +
                     el->x / el->parent->cellWidth  * el->parent->cellWidth;
        el->calc_y = el->parent->calc_y +
                     el->y / el->parent->cellHeight * el->parent->cellHeight;
        break;
      case 2: // absolute grid
        el->calc_x = el->parent->calc_x + el->x * el->parent->cellWidth;
        el->calc_y = el->parent->calc_y + el->y * el->parent->cellHeight;
        break;
      case 3: // horizontal list
        // TODO: update this to make it match vertical list
        if(el->parent->cellWidth) el->calc_w = el->parent->cellWidth;
        else {
          El *child = el->child;
          el->calc_w = 0;
          /* see which child, if any, is the widest. do not nest elements with
          auto dimensions! this is why. */
          while(child) if(child->width > el->calc_w) el->calc_w = child->width;
        }
        if(el->lSib) {
          el->calc_x = el->lSib->calc_x + el->lSib->calc_w;
        } else {
          el->calc_x = el->parent->calc_x;
        }
        // TODO: handle alignments instead of this
        el->calc_y = el->parent->y;
        break;
      case 4: // vertical list
        if(el->parent->cellHeight) {
          el->calc_h = el->parent->cellHeight;
          if(el->lSib) el->calc_y = el->lSib->calc_y + el->parent->cellHeight;
        } else {
          El *child = el->child;
          el->calc_h = 0;
          // see which child, if any, is the tallest.
          while(child) {
            if(child->height > el->calc_h) el->calc_h = child->height;
            child = child->rSib;
          }
          if(el->lSib) el->calc_y = el->lSib->calc_y + el->lSib->calc_h;
        }
        // TODO: handle alignments by changing el->calc_x
        
        break;
      case 5: // sidebar
        el->calc_h = el->parent->calc_h;
        if(el->lSib) { // if this is the second element, its the body
          el->calc_x += 128;
          el->calc_w = el->parent->calc_w - 128;
        } else { // if this is the first element, its the sidebar
          el->calc_w = 128;
        }
        break;
    }
  }
  
  // aaaaand recurse
  if(el->child) {
    el = el->child;
    do {
      el_calc(el);
      el = el->rSib;
    } while(el);
  }
}

void el_draw(El *el, SDL_Renderer *renderer) {
  SDL_Rect el_rect = {
    el->calc_x,
    el->calc_y,
    el->calc_w,
    el->calc_h
  };
  
  printf("drawing el type %i x: %i y: %i w: %i h: %i layout: %i parent:%i\n",
         el->type, el_rect.x, el_rect.y, el_rect.w, el_rect.h, el->layout,
         el->parent ? el->parent->type : 0);
  
  switch(el->type) {
    case 0:
      SDL_SetRenderDrawColor(renderer, 128, 128, 128, 255);
      SDL_RenderClear(renderer);
      break;
    case 2:;
      SDL_Rect handleRect = {
        el->calc_x,
        el->calc_y - 32,
        el->calc_w,
        32
      };
      
      SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255);
      SDL_RenderFillRect(renderer, &handleRect);
      SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
      SDL_RenderDrawRect(renderer, &handleRect);
      
      SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
      SDL_RenderFillRect(renderer, &el_rect);
      SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
      SDL_RenderDrawRect(renderer, &el_rect);
      break;
    default:
      SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
      SDL_RenderFillRect(renderer, &el_rect);
      SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
      SDL_RenderDrawRect(renderer, &el_rect);
      break;
  }
  
  // aaaaand recurse
  if(el->child) {
    printf("has children\n");
    el = el->child;
    do {
      el_draw(el, renderer);
      el = el->rSib;
    } while(el);
  }
}
