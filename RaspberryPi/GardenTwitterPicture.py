'''
Update status with image
https://github.com/ryanmcgrath/twython/blob/master/docs/usage/advanced_usage.rst

'''

from twython import Twython

twitter = Twython(APP_KEY, APP_SECRET,
                  OAUTH_TOKEN, OAUTH_TOKEN_SECRET)

photo = open('images/image.jpg', 'rb')
response = twitter.upload_media(media=photo)
twitter.update_status(status='Checkout this cool image!', media_ids=[response['media_id']])
