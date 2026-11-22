# Practical 02 - Private and Public Members

**Private members** (`accountNumber`, `holderName`, `balance`) hold the account's data. Code outside the
class cannot read or change them directly, so the balance can never be set to an invalid value by mistake.

**Public members** (constructor, `deposit`, `withdraw`, `display`, `setHolderName`, `getBalance`) are the
only way to use the account. Each one validates its input (for example, a deposit must be positive and a
withdrawal cannot exceed the balance) before changing the private data.

This is called **encapsulation**: data is hidden and protected, and the object is used only through
controlled functions.
