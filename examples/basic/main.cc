#include <memory>

#include "p4buf/buffer.h"
#include "p4buf/type_spec.h"

int main(int argc, char *argv[]) {
  // Create a buffer of 2 bytes, with all bytes initialized to 0.
  // Internally a buffer is represented as a flat byte array.
  auto buffer = std::make_shared<p4buf::Buffer>(2, 0);

  // Define some bit fields.
  // 00000000 00000000
  // abbbcccc cddddddd
  p4buf::BufferEditor editor;
  editor.AddField("a", 0, 1);  // The first 1 bit.
  editor.AddField("b", 1, 3);  // The next 3 bits.
  editor.AddField("c", 4, 5);  // The next 5 bits.
  editor.AddField("d", 9, 7);  // The last 7 bits.

  // Edit the buffer.
  editor.Edit(buffer);

  // Set every bit fields to 1.
  editor.GetField("a").Write(p4buf::BitField({0x01}, 1));
  editor.GetField("b").Write(p4buf::BitField({0x01}, 3));
  editor.GetField("c").Write(p4buf::BitField({0x01}, 5));
  editor.GetField("d").Write(p4buf::BitField({0x01}, 7));
  // The buffer becomes:
  // 10010000 10000001
  // abbbcccc cddddddd
  buffer->print();

  // Instead of defining bit fields manually, we could define a data type spec
  // instead. For example we can express the following P4 data type:
  //
  // struct s0_t {
  //     bit<1> a;
  //     bit<3> b;
  // }
  //
  // struct s1_t {
  //     s0_t s0;
  //     bit<5> c;
  //     bit<7> d;
  // }
  p4buf::StructTypeSpec s0_t;
  s0_t.add("a", 1);
  s0_t.add("b", 3);
  p4buf::StructTypeSpec s1_t;
  s1_t.add("s0", s0_t);
  s1_t.add("c", 5);
  s1_t.add("d", 7);

  // Then we can create an editor from this type spec.
  p4buf::BufferEditor editor_from_type(&s1_t);

  // Bit fields are already defined and editable.
  editor_from_type.Edit(buffer);
  editor_from_type.GetField("/s0/a").Write(p4buf::BitField({0b1}, 1));
  editor_from_type.GetField("/s0/b").Write(p4buf::BitField({0b010}, 3));
  editor_from_type.GetField("/c").Write(p4buf::BitField({0b00100}, 5));
  editor_from_type.GetField("/d").Write(p4buf::BitField({0b0001000}, 7));
  // The buffer becomes:
  // 10100010 00001000
  // abbbcccc cddddddd
  buffer->print();

  return 0;
}
