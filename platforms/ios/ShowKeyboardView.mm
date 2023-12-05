//
//  ShowKeyboardView.m
//  Minecraft
//
//  Created by Brent Gibson on 12/4/23.
//
//

#import "ShowKeyboardView.h"

@implementation ShowKeyboardView

UITextField *textField;
NSString *lastString;
Minecraft* _app;

- (id)initWithMinecraft:(Minecraft*)app
{
    self = [self init];
    if (self)
        _app = app;
    return self;
}

- (id)initWithFrame:(CGRect)frame
{
    self = [super initWithFrame:frame];
    if (self) {
        // Initialization code
        textField = [[UITextField alloc] init];
        [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(textFieldDidChange:) name:@"UITextFieldTextDidChangeNotification" object:textField];
        [textField setDelegate:self];
        [textField setAutocorrectionType:UITextAutocorrectionTypeNo];
        [textField setAutocapitalizationType:UITextAutocapitalizationTypeNone];
        [self addSubview:textField];
        lastString = @"AAAA";
        [textField setText:lastString];
    }
    return self;
}

- (BOOL)hasText
{
    return YES;
}

- (BOOL)canBecomeFirstResponder
{
    return YES;
}

- (void)showKeyboard
{
    [textField becomeFirstResponder];
}

- (void)hideKeyboard
{
    [textField resignFirstResponder];
    [self resignFirstResponder];
}

- (void)textFieldDidChange:(NSNotification*)event
{
    UITextField *field = [event object];
    if (field.text != lastString)
    {
        if (lastString.length <= field.text.length)
        {
            if ([field.text characterAtIndex:field.text.length - 1] != '\n')
            {
                if (lastString.length != field.text.length)
                {
                    /*unichar lastUniChar = [field.text characterAtIndex:field.text.length - 1];
                    const char* last = [[NSString stringWithCharacters:&lastUniChar length:1] UTF8String ];
                    if (last)
                    {
                        // Feed new character into handler
                        _app->handleCharInput(last[0]);
                    }*/
                    const char* str = [[field.text substringFromIndex:(field.text.length - (field.text.length - lastString.length))] UTF8String];
                    for (const char *c = str; *c != '\0'; c++)
                    {
                        // Feed new characters into handler
                        _app->handleCharInput(*c);
                    }
                }
                else
                {
                    // Simulate backspace
                    _app->handleCharInput(0x08);
                }
            }
            else
            {
                // Simulate carriage return
                _app->handleCharInput(0x0D);
            }
        }
        else
        {
            // Simulate backspace
            _app->handleCharInput(0x08);
        }
        
        if (textField.text.length < 4)
            textField.text = @"AAAA";
    }
    lastString = [[NSString stringWithString:field.text] retain];
}

- (void)textFieldShouldReturn:(UITextField *)field
{
    // Simulate carriage return
    _app->handleCharInput(0x0D);
}

@end
