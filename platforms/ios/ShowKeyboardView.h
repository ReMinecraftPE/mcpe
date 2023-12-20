//
//  ShowKeyboardView.h
//  Minecraft
//
//  Created by Brent on 12/4/23.
//
//

#import <UIKit/UIKit.h>
#include "Minecraft.hpp"

@interface ShowKeyboardView : UIView<UITextFieldDelegate>
{
    
}
- (id)initWithMinecraft:(Minecraft*)app;
- (BOOL)hasText;
- (BOOL)canBecomeFirstResponder;
- (void)showKeyboard;
- (void)hideKeyboard;
- (void)textFieldDidChange:(NSNotification*)field;
- (void)textFieldShouldReturn:(UITextField *)field;
@end
