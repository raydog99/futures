module DStoreId = struct
  type t = int ref

  let create id = ref id
  let clone t = ref !t
end