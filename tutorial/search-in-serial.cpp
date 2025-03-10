# include "../serialize.h"

const struct Person
{
  const char* name;
  int         id;
} persons[] =
{
  { "Abel", 1 },
  { "Abraham", 2 },
  { "Ada", 3 },
  { "Adalbert", 4 },
  { "Adam", 4 },
  { "Adrian", 6 },
  { "Agatha", 7 },
  { "Agnes", 8 },
  { "Alan", 9 },
  { "Albert", 10 },
  { "Alec", 11 },
  { "Alexander", 12 },
  { "Alfred", 13 },
  { "Algernon", 14 },
  { "Alice", 15 },
  { "Allan", 16 },
  { "Aloys", 17 },
  { "Ambrose", 18 },
  { "Amelia", 19 },
  { "Amy", 20 },
  { "Anabel", 21 },
  { "Andrew", 22 },
  { "Andromache", 23 },
  { "Andy", 24 },
  { "Angelica", 25 },
  { "Angelina", 26 },
  { "Ann", 27 },
  { "Annabel", 28 },
  { "Annie", 29 },
  { "Antoinette", 30 },
  { "Antony", 31 },
  { "Arabella", 32 },
  { "Archibald", 33 },
  { "Archie", 34 },
  { "Arnold", 35 },
  { "Arthur", 36 },
  { "Aubrey", 37 },
  { "August", 38 },
  { "Augustus", 39 },
  { "Aurora", 40 },
  { "Austin", 41 },
  { "Bab", 42 },
  { "Baldwin", 43 },
  { "Barbara", 44 },
  { "Bart", 45 },
  { "Bartholomew", 46 },
  { "Basil", 47 },
  { "Beatrice", 48 },
  { "Beatrix", 49 },
  { "Beck", 50 },
  { "Bel", 51 },
  { "Ben", 52 },
  { "Benedict", 53 },
  { "Benjamin", 54 },
  { "Benny", 55 },
  { "Bernard", 56 },
  { "Bert", 57 },
  { "Bertie", 58 },
  { "Bertram", 59 },
  { "Bessie", 60 },
  { "Betsey", 61 },
  { "Betty", 62 },
  { "Biddy", 63 },
  { "Bill", 64 },
  { "Blanch", 65 },
  { "Bobbie", 66 },
  { "Borland", 67 },
  { "Brian", 68 },
  { "Bridget", 69 },
  { "Candida", 70 },
  { "Carol", 71 },
  { "Caroline", 72 },
  { "Carrie", 73 },
  { "Caspar", 74 },
  { "Catherine", 75 },
  { "Cecil", 76 },
  { "Cecilia", 77 },
  { "Charlesey", 78 },
  { "Charlotte", 79 },
  { "Chris", 80 },
  { "Christian", 81 },
  { "Christiana", 82 },
  { "Christina", 83 },
  { "Christopher", 84 },
  { "Christy", 85 },
  { "Clara", 86 },
  { "Clarence", 87 },
  { "Clark", 88 },
  { "Claud", 89 },
  { "Claudius", 90 },
  { "Clem", 91 },
  { "Clement", 92 },
  { "Clementina", 93 },
  { "Clifford", 94 },
  { "Clotilda", 95 },
  { "Colette", 96 },
  { "Connie", 97 },
  { "Connor", 98 },
  { "Constance", 99 },
  { "Cora", 100 },
  { "Cordelia", 101 },
  { "Cornelia", 102 },
  { "Cornelius", 103 },
  { "Craig", 104 },
  { "Cyril", 105 },
  { "Cyrus", 106 },
  { "Dan", 107 },
  { "Daniel", 108 },
  { "Dannie", 109 },
  { "Dave", 110 },
  { "David", 111 },
  { "Davy", 112 },
  { "Deborah", 113 },
  { "Denis", 114 },
  { "Desmond", 115 },
  { "Diana", 116 },
  { "Dick", 117 },
  { "Dickie", 118 },
  { "Dickon", 119 },
  { "Dinah", 120 },
  { "Dob", 121 },
  { "Doll", 122 },
  { "Dolores", 123 },
  { "Donald", 124 },
  { "Dora", 125 },
  { "Dorian", 126 },
  { "Doris", 127 },
  { "Dorothy", 128 },
  { "Douglas", 129 },
  { "Ed", 130 },
  { "Eddie", 131 },
  { "Edgar", 132 },
  { "Edith", 133 },
  { "Edmund", 134 },
  { "Edna", 135 },
  { "Edward", 136 },
  { "Edwin", 137 },
  { "Eleanor", 138 },
  { "Elijah", 139 },
  { "Elisabeth", 140 },
  { "Ella", 141 },
  { "Ellen", 142 },
  { "Elliot", 143 },
  { "Elmer", 144 },
  { "Elsie", 145 },
  { "Elvira", 146 },
  { "Em", 147 },
  { "Emery", 148 },
  { "Emilia", 149 },
  { "Emm", 150 },
  { "Emmanuel", 151 },
  { "Emmie", 152 },
  { "Enoch", 153 },
  { "Erasmus", 154 },
  { "Ernest", 155 },
  { "Ernie", 156 },
  { "Essie", 157 },
  { "Ethel", 158 },
  { "Etta", 159 },
  { "Eugene", 160 },
  { "Eustace", 161 },
  { "Eva", 162 },
  { "Evelina", 163 },
  { "Fanny", 164 },
  { "Felicia", 165 },
  { "Felix", 166 },
  { "Ferdinand", 167 },
  { "Fidelia", 168 },
  { "Flo", 169 },
  { "Flora", 170 },
  { "Florence", 171 },
  { "Flossie", 172 },
  { "Floy", 173 },
  { "Frances", 174 },
  { "Francis", 175 },
  { "Frank", 176 },
  { "Freda", 177 },
  { "Freddie", 178 },
  { "Frederic", 179 },
  { "Gabriel", 180 },
  { "Geffrey", 181 },
  { "George", 182 },
  { "Gerald", 183 },
  { "Gerhard", 184 },
  { "Gertie", 185 },
  { "Gertrude", 186 },
  { "Gideon", 187 },
  { "Gil", 188 },
  { "Gilbert", 189 },
  { "Gladys", 190 },
  { "Gloria", 191 },
  { "Godfrey", 192 },
  { "Godwin", 193 },
  { "Gordon", 194 },
  { "Grace", 195 },
  { "Graham", 196 },
  { "Gregory", 197 },
  { "Greta", 198 },
  { "Griffith", 199 },
  { "Guy", 200 },
  { "Gwendolen", 201 },
  { "Hadrian", 202 },
  { "Hal", 203 },
  { "Hannah", 204 },
  { "Harold", 205 },
  { "Harriet", 206 },
  { "Harry", 207 },
  { "Hatty", 208 },
  { "Helen", 209 },
  { "Henrietta", 210 },
  { "Henry", 211 },
  { "Herbert", 212 },
  { "Herman", 213 },
  { "Hetty", 214 },
  { "Hilary", 215 },
  { "Hilda", 216 },
  { "Hope", 217 },
  { "Horace", 218 },
  { "Howard", 219 },
  { "Hubert", 220 },
  { "Hugh", 221 },
  { "Humphry", 222 },
  { "Ida", 223 },
  { "Ira", 224 },
  { "Irene", 225 },
  { "Isaac", 226 },
  { "Isabel", 227 },
  { "Isaiah", 228 },
  { "Isidore", 229 },
  { "Isold", 230 },
  { "Israel", 231 },
  { "Jack", 232 },
  { "Jacob", 233 },
  { "Jake", 234 },
  { "James", 235 },
  { "Jane", 236 },
  { "Janet", 237 },
  { "Jason", 238 },
  { "Jasper", 239 },
  { "Jean", 240 },
  { "Jeff", 241 },
  { "Jeffrey", 242 },
  { "Jem", 243 },
  { "Jemima", 244 },
  { "Jen", 245 },
  { "Jennifer", 246 },
  { "Jenny", 247 },
  { "Jeremiah", 248 },
  { "Jerome", 249 },
  { "Jerry", 250 },
  { "Jess", 251 },
  { "Jessica", 252 },
  { "Jessie", 253 },
  { "Jim", 254 },
  { "Jo", 2255 },
  { "Joachim", 256 },
  { "Joan", 257 },
  { "Jock", 258 },
  { "Joe", 259 },
  { "Joey", 260 },
  { "John", 261 },
  { "Johnny", 262 },
  { "Jonathan", 263 },
  { "Joseph", 263 },
  { "Josephine", 264 },
  { "Joshua", 266 },
  { "Joy", 267 },
  { "Joyce", 267 },
  { "Jozy", 268 },
  { "Judith", 270 },
  { "Judy", 271 },
  { "Julia", 272 },
  { "Julian", 273 },
  { "Juliana", 274 },
  { "Juliet", 275 },
  { "Julius", 276 },
  { "Kate", 277 },
  { "Katharine", 278 },
  { "Kathleen", 279 },
  { "Katrine", 280 },
  { "Keith", 281 },
  { "Kenneth", 282 },
  { "Kit", 283 },
  { "Kitty", 284 },
  { "Lambert", 285 },
  { "Laura", 286 },
  { "Laurence", 287 },
  { "Lauretta", 288 },
  { "Lazarus", 289 },
  { "Leila", 290 },
  { "Leo", 291 },
  { "Leonard", 292 },
  { "Leonora", 293 },
  { "Leopold", 294 },
  { "Lesley", 295 },
  { "Lew", 296 },
  { "Lewis", 297 },
  { "Lillian", 298 },
  { "Lily", 299 },
  { "Linda", 300 },
  { "Lionel", 301 },
  { "Liza", 302 },
  { "Lola", 303 },
  { "Lolly", 304 },
  { "Lottie", 305 },
  { "Louie", 306 },
  { "Louis", 307 },
  { "Louisa", 308 },
  { "Lucas", 309 },
  { "Lucy", 310 },
  { "Luke", 311 },
  { "Mabel", 312 },
  { "Madeleine", 313 },
  { "Madge", 314 },
  { "Mag", 315 },
  { "Maggie", 316 },
  { "Magnus", 317 },
  { "Malcolm", 318 },
  { "Mamie", 319 },
  { "Marcus", 320 },
  { "Margaret", 321 },
  { "Margery", 322 },
  { "Margie", 323 },
  { "Maria", 324 },
  { "Marian", 325 },
  { "Marina", 326 },
  { "Marion", 327 },
  { "Marjory", 328 },
  { "Mark", 329 },
  { "Martha", 330 },
  { "Martin", 331 },
  { "Mary", 332 },
  { "Mat", 333 },
  { "Matilda", 334 },
  { "Matthew", 335 },
  { "Matty", 336 },
  { "Maud", 337 },
  { "Maurice", 338 },
  { "Max", 339 },
  { "Maximilian", 340 },
  { "Meg", 341 },
  { "Mercy", 342 },
  { "Meredith", 343 },
  { "Michael", 344 },
  { "Micky", 345 },
  { "Mike", 346 },
  { "Mildred", 347 },
  { "Millie", 348 },
  { "Mima", 349 },
  { "Minna", 350 },
  { "Minnie", 351 },
  { "Mirabel", 352 },
  { "Miranda", 353 },
  { "Miriam", 354 },
  { "Moll", 355 },
  { "Monica", 356 },
  { "Montagu", 357 },
  { "Monty", 358 },
  { "Morgan", 359 },
  { "Morris", 360 },
  { "Mortimer", 361 },
  { "Moses", 362 },
  { "Muriel", 363 },
  { "Nance", 364 },
  { "Nannie", 365 },
  { "Nat", 366 },
  { "Natalia", 367 },
  { "Nathan", 368 },
  { "Nathaniel", 369 },
  { "Neddie", 370 },
  { "Nellie", 371 },
  { "Nettie", 372 },
  { "Neville", 373 },
  { "Nicholas", 374 },
  { "Nik", 375 },
  { "Nikola", 376 },
  { "Ninette", 377 },
  { "Noah", 378 },
  { "Noel", 379 },
  { "Noll", 380 },
  { "Nora", 381 },
  { "Norman", 382 },
  { "Odette", 383 },
  { "Olive", 384 },
  { "Oliver", 385 },
  { "Olivia", 386 },
  { "Ophelia", 387 },
  { "Oscar", 388 },
  { "Osmond", 389 },
  { "Oswald", 390 },
  { "Ottilia", 391 },
  { "Owen", 392 },
  { "Paddy", 393 },
  { "Pat", 394 },
  { "Patricia", 395 },
  { "Patrick", 396 },
  { "Patty", 397 },
  { "Paul", 398 },
  { "Paula", 399 },
  { "Paulina", 400 },
  { "Peg", 401 },
  { "Pen", 402 },
  { "Penelope", 403 },
  { "Penny", 404 },
  { "Persy", 405 },
  { "Pete", 406 },
  { "Peter", 407 },
  { "Phil", 408 },
  { "Philip", 409 },
  { "Pip", 410 },
  { "Pius", 411 },
  { "Pol", 412 },
  { "Portia", 413 },
  { "Rachel", 414 },
  { "Ralph", 415 },
  { "Ranald", 416 },
  { "Randolph", 417 },
  { "Raphael", 418 },
  { "Rasmus", 419 },
  { "Ray", 420 },
  { "Raymond", 421 },
  { "Rebecca", 422 },
  { "Reg", 423 },
  { "Reginald", 424 },
  { "Reynold", 425 },
  { "Richard", 426 },
  { "Rita", 427 },
  { "Rob", 428 },
  { "Robert", 429 },
  { "Robin", 430 },
  { "Roddy", 431 },
  { "Roderick", 432 },
  { "Rodney", 433 },
  { "Roger", 434 },
  { "Roland", 435 },
  { "Rolf", 436 },
  { "Romeo", 437 },
  { "Ronald", 438 },
  { "Rosa", 439 },
  { "Rosabel", 440 },
  { "Rosalia", 441 },
  { "Rosalind", 442 },
  { "Rosamond", 443 },
  { "Rose", 444 },
  { "Rosemary", 445 },
  { "Roy", 446 },
  { "Rudolf", 447 },
  { "Rupert", 448 },
  { "Ruth", 449 },
  { "Sadie", 450 },
  { "Sal", 451 },
  { "Salome", 452 },
  { "Sam", 453 },
  { "Samson", 454 },
  { "Samuel", 455 },
  { "Sanders", 456 },
  { "Sandy", 457 },
  { "Sara", 458 },
  { "Saul", 459 },
  { "Sebastian", 460 },
  { "Septimus", 461 },
  { "Sibil", 462 },
  { "Sidney", 463 },
  { "Siegfried", 464 },
  { "Silas", 465 },
  { "Silvester", 466 },
  { "Silvia", 467 },
  { "Sim", 468 },
  { "Simeon", 469 },
  { "Simmy", 470 },
  { "Simon", 471 },
  { "Sol", 472 },
  { "Solomon", 473 },
  { "Sophia", 474 },
  { "Sophie", 475 },
  { "Stanislas", 476 },
  { "Stanley", 477 },
  { "Stella", 478 },
  { "Stephana", 479 },
  { "Stephen", 480 },
  { "Sue", 481 },
  { "Susanna", 482 },
  { "Susie", 483 },
  { "Ted", 484 },
  { "Teresa", 485 },
  { "Terry", 486 },
  { "Tessa", 487 },
  { "Theobald", 488 },
  { "Theodora", 489 },
  { "Theodore", 490 },
  { "Thomas", 491 },
  { "Tib", 492 },
  { "Tilda", 493 },
  { "Tilly", 494 },
  { "Tim", 495 },
  { "Timothy", 496 },
  { "Tina", 497 },
  { "Tobias", 498 },
  { "Toby", 499 },
  { "Tom", 500 },
  { "Tommy", 501 },
  { "Tony", 502 },
  { "Tristan", 503 },
  { "Trudy", 504 },
  { "Tybalt", 505 },
  { "Valentine", 506 },
  { "Veronica", 507 },
  { "Victor", 508 },
  { "Victoria", 509 },
  { "Vincent", 510 },
  { "Viola", 511 },
  { "Violet", 512 },
  { "Virginia", 513 },
  { "Vivian", 514 },
  { "Wallace", 515 },
  { "Walt", 516 },
  { "Walter", 517 },
  { "Wat", 518 },
  { "Wilfred", 519 },
  { "Will", 519 },
  { "William", 520 },
  { "Willy", 521 },
  { "Win", 523 },
  { "Winifred", 524 },
  { "Winnie", 525 }
};

template <size_t N>
size_t  GetBufLen( const Person (&names)[N] )
{
  size_t  length = sizeof(uint16_t) * (N + 1);

  for ( auto& next: names )
    length += ::GetBufLen( next.id ) + 1 + strlen( next.name );

  return length;
}

template <class O>
O*  PutWord16( O* o, uint16_t u )
{
  uint8_t  abytes[2] = { uint8_t(u), uint8_t(u >> 8) };

  return ::Serialize( o, abytes, 2 );
}

template <class S>
auto  GetWord16( S* s ) -> uint16_t
{
  uint8_t  abytes[2];

  ::FetchFrom( s, abytes, 2 );

  return uint16_t(abytes[0]) | (uint16_t(abytes[1]) << 8);
}

template <class O, size_t N>
O*  Serialize( O* o, const Person (&names)[N] )
{
  uint16_t  offset = sizeof(uint16_t) * (1 + N);

  o = ::PutWord16( o, uint16_t(N) );

  for ( auto& next: names )
  {
    o = ::PutWord16( o, offset );
    offset += ::GetBufLen( next.id ) + 1 + strlen( next.name );
  }

  for ( auto& next: names )
    o = ::Serialize( ::Serialize( o, next.name, strlen( next.name ) + 1 ), next.id );

  return o;
}

# include <algorithm>
# include <cstring>
# include <chrono>

template <class T, size_t N>
auto  array_size( T (&)[N] ) -> size_t  {  return N;  }

template <class Search>
auto  Measure( Search search, size_t loops = 1000000 ) -> std::pair<unsigned, unsigned>
{
  using namespace std::chrono;

  auto  start = high_resolution_clock::now();
  auto  found = unsigned(0);
  auto  count = array_size(persons);

  for ( unsigned i = 0; i < loops; ++i )
  {
    auto  nindex = rand() % count;
    auto  bfound = search( persons[nindex].name );

    if ( bfound )
      ++found;
  }

  return { duration_cast<milliseconds>( high_resolution_clock::now() - start ).count(), found };
}

# include <cstdio>
# include <vector>

int   main()
{
  auto  serial = std::vector<char>( ::GetBufLen( persons ) );

  Serialize( serial.data(), persons );

  auto  linear = Measure( []( const char* s )
  {
    auto  element = std::find_if( persons, persons + array_size(persons),
      [&]( const Person& p ){  return strcmp( p.name, s ) == 0;  } );

    return element != persons + array_size(persons);
  } );
  auto  lbound = Measure( []( const char* s )
  {
    auto  element = std::lower_bound( persons, persons + array_size(persons), s,
      []( const Person& p, const char* n ){  return strcmp( p.name, n ) < 0;  } );

    return element != persons + array_size(persons) && strcmp( element->name, s ) == 0;
  } );
  auto  binary = Measure( []( const char* s )
  {
    auto  beg = persons;
    auto  end = persons + array_size(persons);

    while ( beg <= end )
    {
      auto  mid = beg + (end - beg) / 2;
      auto  cmp = strcmp( s, mid->name );

      if ( cmp < 0 )  end = mid - 1;
        else
      {
        if ( cmp == 0 )
          return true;
        beg = mid + 1;
      }
    }
    return false;
  } );
  auto  binbuf = Measure( [&]( const char* s )
  {
    auto  origin = (const char*)serial.data();
    auto  length = GetWord16( origin );
    auto  lbound = origin + sizeof(uint16_t);
    auto  hbound = lbound + length * sizeof(uint16_t);

    while ( lbound <= hbound )
    {
      auto  middle = lbound + ((hbound - lbound) / 2 / sizeof(uint16_t)) * sizeof(uint16_t);
      auto  midofs = GetWord16( (const char*)middle );
      auto  midrec = origin + midofs;
      auto  rescmp = strcmp( s, midrec );

      if ( rescmp < 0 ) hbound = middle - sizeof(uint16_t);
        else
      {
        if ( rescmp == 0 )
          return true;
        lbound = middle + sizeof(uint16_t);
      }
    }
    return false;
  } );

  fprintf( stdout, "blob size: %u\n", unsigned(serial.size()) );

  fprintf( stdout, "std::find_if:     found %u: %u ms\n", linear.second, linear.first );
  fprintf( stdout, "std::lower_bound: found %u: %u ms\n", lbound.second, lbound.first );
  fprintf( stdout, "simple binsearch: found %u: %u ms\n", binary.second, binary.first );
  fprintf( stdout, "serial binsearch: found %u: %u ms\n", binbuf.second, binbuf.first );

  return 0;
}
